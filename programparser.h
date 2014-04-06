#ifndef PROGRAMPARSER_H
#define PROGRAMPARSER_H

#include <memory>

#include <QString>

class Number;
class ParseError;
class Program;
class ProgramCommand;
class Routine;


struct TextPosition
{
    TextPosition() : line(-1), column(0) {}
    TextPosition(int line_, int column_) : line(line_), column(column_) {}

    int line;
    int column;
};


class ProgramParser
{
public:
    ProgramParser();
    ~ProgramParser();

    void processLine(const QString& nextLine);
    std::unique_ptr<Program> finish();

private:
    enum class Section {
        MainHeader1,
        MainHeader2,
        MainBody,
        FirstSubroutineHeader,
        SuccessiveSubroutineHeader,
        SubroutineBody,
        End,
    };

private:
    static const int infinity; //TODO// = std::numeric_limits<int>::max();

private:
    TextPosition m_position;
    QString m_line;
    Section m_section;
    std::unique_ptr<Program> m_program;
    int m_currentRoutineIndex;

private:
    ParseError frustratedExpectations(const QString &expectation) const;

    void setPosition(TextPosition position);
    void setCurrentRoutineIndex(int newIndex);
    void setSection(Section newSection);
    bool atEol() const;
    QStringRef line() const;
    QStringRef prefix(int length) const;
    QChar nextChar() const;
    void advance(int nChars);

    QChar eatChar();
    void eat(const QString& text);
    bool tryToEat(const QString& text);
    int eatUnsignedInteger(int length = infinity);
    Number eatNumber();
    void eatLineNo();

    void pushCommand(std::unique_ptr<ProgramCommand> newCommand);

    void parseSubroutineHeader();
    void parseStatement();
    void parseSubroutineCall();
    void parseCommand();
    void parseControlCommand(bool &subroutineFinished);
    void parseSetWidthCommand();
    void parseMovementCommand();
    void parseSubroutineEnd();

private:
    friend class EolChecker;
};

#endif // PROGRAMPARSER_H
