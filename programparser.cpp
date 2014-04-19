// TODO: Add function make_command that acts like make unique, but also saves current positino to command; use it in execution error messages

#include <assert.h>

#include "cpp_extensions.h"
#include "program.h"
#include "programparser.h"
#include "programcommands.h"


const int ProgramParser::infinity = std::numeric_limits<int>::max();


class CursorTracker
{
public:
    CursorTracker(const ProgramParser& parser) : m_parser(parser), m_begin (parser.m_position) {}
    TextRange range() { return TextRange(m_begin, m_parser.m_position); }

private:
    const ProgramParser& m_parser;
    TextPosition m_begin;
};


ParseError::ParseError(TextPosition positionArg, const QString& whatArg)
    : std::runtime_error(QString("Строка %1, символ %2: %3").arg(QString::number(positionArg.line + 1), QString::number(positionArg.column + 1), whatArg).toStdString())
    , m_position(positionArg)
{
}


ProgramParser::ProgramParser()
    : m_section(Section::MainHeader1)
    , m_program(new Program)
    , m_currentRoutineIndex(mainRoutineIndex)
{
}

ProgramParser::~ProgramParser()
{
}

void ProgramParser::processLine(const QString& nextLine)
{
    assert(m_program);
    setPosition(TextPosition(m_position.line + 1, 0));
    m_line = nextLine.trimmed();
    if (atEol())
        return;

    switch (m_section)
    {
    case Section::MainHeader1:
        eat("%1G01G91G92M77");
        setSection(Section::MainHeader2);
        break;
    case Section::MainHeader2:
        eatLineNo();
        eat("F1200");
        setSection(Section::MainBody);
        break;
    case Section::MainBody:
        eatLineNo();
        if (tryToEat("M02"))
            setSection(Section::FirstSubroutineHeader);
        else
            parseStatement();
        break;
    case Section::FirstSubroutineHeader:
        eat("%2");
        if (atEol()) {
            setSection(Section::SuccessiveSubroutineHeader);
            break;
        }
        parseSubroutineHeader();
        break;
    case Section::SuccessiveSubroutineHeader:
        parseSubroutineHeader();
        break;
    case Section::SubroutineBody:
        eatLineNo();
        parseStatement();
        break;
    case Section::End:
        throw ParseError(m_position, "Неожиданный текст после конца программы");
        break;
    }

    if (!atEol())
        throw frustratedExpectations("конец строки");
}

std::unique_ptr<Program> ProgramParser::finish()
{
    if (m_section != Section::End)
        throw ParseError(m_position, QString("Неожиданный конец программы"));
    return std::move(m_program);
}

ParseError ProgramParser::frustratedExpectations(const QString& expectation) const
{
    return ParseError(m_position, QString("Ожидалось %1, получено \"%2\"").arg(expectation, line().toString()));
}

void ProgramParser::setPosition(TextPosition position)
{
    m_position = position;
}

void ProgramParser::setCurrentRoutineIndex(int newIndex)
{
    m_currentRoutineIndex = newIndex;
}

void ProgramParser::setSection(Section newSection)
{
    m_section = newSection;
}

bool ProgramParser::atEol() const
{
    return m_position.column >= m_line.length();
}

QStringRef ProgramParser::line() const
{
    return m_line.midRef(m_position.column);
}

QStringRef ProgramParser::prefix(int length) const
{
    return m_line.midRef(m_position.column, length);
}

QChar ProgramParser::nextChar() const
{
    return m_position.column < m_line.length() ? m_line[m_position.column] : QChar('\0');
}

void ProgramParser::advance(int nChars)
{
    setPosition(TextPosition(m_position.line, m_position.column + nChars));
    if (m_position.column > m_line.length())
        throw ParseError(m_position, "Неожиданный конец строки");
}

QChar ProgramParser::eatChar()
{
    QChar result = nextChar();
    advance(1);
    return result;
}

void ProgramParser::eat(const QString& text)
{
    if (!tryToEat(text))
        throw frustratedExpectations(QString("\"%1\"").arg(text));
}

bool ProgramParser::tryToEat(const QString& text)
{
    if (line().startsWith(text)) {
        advance(text.length());
        return true;
    }
    return false;
}

int ProgramParser::eatUnsignedInteger(int length)
{
    int nDigits = 0;
    while (nDigits < line().length() && nDigits < length && line().at(nDigits).isDigit())
        ++nDigits;
    if (length != infinity && nDigits != length)
        throw frustratedExpectations(QString("%1-значное число").arg(QString::number(length)));
    if (nDigits == 0)
        throw frustratedExpectations("число");
    bool ok = false;
    int result = prefix(nDigits).toString().toInt(&ok);
    if (!ok)
        throw ParseError(m_position, QString("Недопустимое число: \"%1\"").arg(line().toString()));
    advance(nDigits);
    return result;
}

Number ProgramParser::eatNumber()
{
    int sign = 1;
    if (nextChar() == '+' || nextChar() == '-')
        sign = (eatChar() == '-') ? -1 : 1;
    if (tryToEat("R"))
        return Number::variable(eatUnsignedInteger(), sign);
    return Number::literal(eatUnsignedInteger() * sign);
}

void ProgramParser::eatLineNo()
{
    eat("N");
    eatUnsignedInteger();
}

void ProgramParser::pushCommand(TextRange textRange, std::unique_ptr<ProgramCommand> newCommand)
{
    newCommand->setTextRange(textRange);
    m_program->pushBack(m_currentRoutineIndex, std::move(newCommand));
}

void ProgramParser::parseSubroutineHeader()
{
    if (nextChar() == 'L') {
        eat("L");
        int subroutineIndex = eatUnsignedInteger();
        setCurrentRoutineIndex(subroutineIndex);
        setSection(Section::SubroutineBody);
    }
    else {
        eatLineNo();
        eat("M30");
        setSection(Section::End);
        return;
    }
}

void ProgramParser::parseStatement()
{
    if (nextChar() == 'L')
        parseSubroutineCall();
    else
        parseCommand();
}

void ProgramParser::parseSubroutineCall()
{
    CursorTracker cursorTracker(*this);
    eat("L");
    int subroutineIndex = eatUnsignedInteger(2);
    int repeatCount = eatUnsignedInteger(2);
    Arguments arguments;
    while (!atEol()) {
        eat("R");
        int iArg = eatUnsignedInteger(1);
        eat("+");
        int argValue = eatUnsignedInteger();
        arguments.set(iArg, argValue);
    }
    pushCommand(cursorTracker.range(), std::make_unique<CallSubroutineCommand>(subroutineIndex, repeatCount, arguments));
}

void ProgramParser::parseCommand()
{
    while (nextChar() == 'M' || nextChar() == '(') {
        bool subroutineFinished = false;
        if (nextChar() == 'M')
            parseControlCommand(subroutineFinished);
        else if (nextChar() == '(')
            parseSetWidthCommand();
        if (subroutineFinished)
            return;
    }
    parseMovementCommand();
}

void ProgramParser::parseControlCommand(bool& subroutineFinished)
{
    CursorTracker cursorTracker(*this);
    subroutineFinished = false;
    if (tryToEat("M77"))
        pushCommand(cursorTracker.range(), std::make_unique<DisableLaserCommand>());
    else if (tryToEat("M78"))
        pushCommand(cursorTracker.range(), std::make_unique<EnableLaserCommand>());
    else if (m_section == Section::SubroutineBody && tryToEat("M17")) {
        subroutineFinished = true;
        setSection(Section::SuccessiveSubroutineHeader);
    }
    else
        throw frustratedExpectations("\"M77\", \"M78\" или \"M17\"");
}

void ProgramParser::parseSetWidthCommand()
{
    CursorTracker cursorTracker(*this);
    eat("(");
    int newWidth = eatUnsignedInteger();
    eat(")");
    pushCommand(cursorTracker.range(), std::make_unique<SetLineWidthCommand>(newWidth));
}

void ProgramParser::parseMovementCommand()
{
    CursorTracker cursorTracker(*this);
    QString moveTwiceMessage("перемещание по \"%1\" задано дважды в одной операции");
    bool xSet = false, ySet = false;
    Movement movement;
    while (!atEol()) {
        if (tryToEat("X")) {
            if (xSet)
                throw ParseError(m_position, moveTwiceMessage.arg("X"));
            movement.x = eatNumber();
            xSet = true;
        }
        else if (tryToEat("Y")) {
            if (ySet)
                throw ParseError(m_position, moveTwiceMessage.arg("Y"));
            movement.y = eatNumber();
            ySet = true;
        }
        else
            throw frustratedExpectations("\"X\" или \"Y\"");
    }
    pushCommand(cursorTracker.range(), std::make_unique<MoveToCommand>(movement));
}
