#ifndef PROGRAMPARSER_H
#define PROGRAMPARSER_H

#include <memory>

#include <QString>

#include "programbasic.h"

class CursorTracker;
class Number;
class ParseError;
class Program;
class ProgramCommand;
class Routine;


class ParseError : public std::runtime_error
{
public:
    ParseError(TextPosition positionArg, const QString& whatArg);
    TextPosition position() const   { return m_position; }

private:
    TextPosition m_position;
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
    static const int infinity;

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

    void pushCommand(TextRange textRange, std::unique_ptr<ProgramCommand> newCommand);

    void parseSubroutineHeader();
    void parseStatement();
    void parseSubroutineCall();
    void parseCommand();
    void parseControlCommand(bool &subroutineFinished);
    void parseSetWidthCommand();
    void parseMovementCommand();
    void parseSubroutineEnd();

private:
    friend class CursorTracker;
};

#endif // PROGRAMPARSER_H
