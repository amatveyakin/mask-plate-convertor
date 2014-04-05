#include <assert.h>

#include "cpp_extensions.h"
#include "program.h"
#include "programparser.h"


class ParseError : public std::runtime_error
{
public:
    ParseError(TextPosition position, const QString& what_arg)
        : std::runtime_error(QString("[%1:%2] %3").arg(QString::number(position.line), QString::number(position.column), what_arg).toStdString()) {}
};


ProgramParser::ProgramParser()
    : m_program(new Program)
    , m_currentRoutineIndex(Program::mainRoutineIndex)
{
}

ProgramParser::~ProgramParser()
{
}

void ProgramParser::processLine(const QString& nextLine)
{
    assert(m_program);
    setPosition({m_position.line + 1, 0});
    m_line = nextLine.trimmed();
    if (atEol())
        return;
    switch (m_section)
    {
    case Section::MainHeader1:
        parseFixedLine("%1G01G91G92M77");
        startNextSection();
        return;
    case Section::MainHeader2:
        eatLineNo();
        parseFixedLine("F1200");
        startNextSection();
        return;
    case Section::MainBody:
        eatLineNo();
        if (line().startsWith("M02"))
            startNextSection();
        else
            parseStatement();
        return;
    case Section::SubroutinesHeader:
        parseFixedLine("%2L51");
        startNextSection();
        return;
    case Section::SubroutinesBody:
        if (nextChar() == 'L')
            parseSubroutineHeader();
        else {
            eatLineNo();
            parseStatement();
        }
        return;
    }
}

std::unique_ptr<Program> ProgramParser::finish()
{
    if (m_section != Section::SubroutinesBody)
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

void ProgramParser::startNextSection()
{
    m_section = Section(int(m_section) + 1);
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
    setPosition({m_position.line, m_position.column + nChars});
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
    if (nextChar() != '+' && nextChar() != '-')
        throw frustratedExpectations("\"+\" или \"-\"");
    int sign = (eatChar() == '-') ? -1 : 1;
    if (tryToEat("R"))
        return Number::variable(eatUnsignedInteger() * sign);
    return Number::literal(eatUnsignedInteger() * sign);
}

void ProgramParser::eatLineNo()
{
    eat("N");
    eatUnsignedInteger();
}

void ProgramParser::pushCommand(std::unique_ptr<ProgramCommand> newCommand)
{
    m_program->pushBack(m_currentRoutineIndex, std::move(newCommand));
}

void ProgramParser::parseSubroutineHeader()
{
    eat("L");
    int subroutineIndex = eatUnsignedInteger();
    setCurrentRoutineIndex(subroutineIndex);
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
    pushCommand(std::make_unique<CallSubroutineCommand>(subroutineIndex, repeatCount, arguments));
}

void ProgramParser::parseCommand()
{
    while (nextChar() == 'M' || nextChar() == '(') {
        if (nextChar() == 'M')
            parseLaserSwitchCommand();
        else if (nextChar() == '(')
            parseSetWidthCommand();
    }
    parseMovementCommand();
}

void ProgramParser::parseLaserSwitchCommand()
{
    eat("M");
    if (tryToEat("77"))
        pushCommand(std::make_unique<DisableLaserCommand>());
    else if (tryToEat("78"))
        pushCommand(std::make_unique<EnableLaserCommand>());
    else
        throw frustratedExpectations("\"77\" или \"78\"");
}

void ProgramParser::parseSetWidthCommand()
{
    eat("(");
    int newWidth = eatUnsignedInteger();
    eat(")");
    pushCommand(std::make_unique<SetLineWidthCommand>(newWidth));
}

void ProgramParser::parseMovementCommand()
{
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
    pushCommand(std::make_unique<MoveToCommand>(movement));
}

void ProgramParser::parseFixedLine(const QString& expected)
{
    eat(expected);
    if (!atEol())
        throw ParseError(m_position, QString("Неожиданный текст: \"%1\"").arg(line().toString()));
}
