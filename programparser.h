#ifndef PROGRAMPARSER_H
#define PROGRAMPARSER_H

#include <QString>

class Program;

class ProgramParser
{
public:
    ProgramParser(Program* program);
    ~ProgramParser();

    void processLine(const QString& line);

private:
    enum class Section {
        MainHeader1,
        MainHeader2,
        MainBody,
        SubroutinesHeader,
        SubroutinesBody,
    };

private:
    Section m_section  = Section::MainHeader1;
    Program* m_program = nullptr;
};

#endif // PROGRAMPARSER_H
