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
    TextPosition() {}
    TextPosition(int line_, int column_) : line(line_), column(column_) {}
    int line = -1;
    int column = 0;
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
        SubroutinesHeader,
        SubroutinesBody,
    };

private:
    static const int infinity = std::numeric_limits<int>::max();

private:
    TextPosition m_position;
    QString m_line;
    Section m_section = Section::MainHeader1;
    std::unique_ptr<Program> m_program;
    int m_currentRoutineIndex;

private:
    ParseError frustratedExpectations(const QString &expectation) const;

    void setPosition(TextPosition position);
    void setCurrentRoutineIndex(int newIndex);
    void startNextSection();
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
    void parseLaserSwitchCommand();
    void parseSetWidthCommand();
    void parseMovementCommand();
    void parseFixedLine(const QString& expected);
};

#endif // PROGRAMPARSER_H
