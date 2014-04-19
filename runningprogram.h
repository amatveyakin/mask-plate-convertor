#ifndef RUNNINGPROGRAM_H
#define RUNNINGPROGRAM_H

#include <memory>
#include <vector>

#include "programbasic.h"

class Blueprint;
class Program;
class ProgramCommand;


class ExecutionError : public std::runtime_error
{
public:
    ExecutionError(TextRange commandTextRangeArg, const std::string& callStackDump, const std::string& whatArg);

    TextRange commandTextRange() const  { return m_commandTextRange; }

private:
    TextRange m_commandTextRange;
};


struct Call
{
    Call(int routineIndexArg) : routineIndex(routineIndexArg) {}

    int routineIndex;
};


class CallStack : public std::vector<Call>
{
public:
    std::string dump() const;
};


struct ProgramState
{
    ProgramState() : currentCommand(nullptr), laserEnabled(false), lineWidth(startingLineWidth) {}

    const ProgramCommand *currentCommand;
    CallStack callStack;
    Arguments arguments;
    bool laserEnabled;
    int lineWidth;
    QPoint position;
};


struct RunningProgram
{
    RunningProgram(const Program *programArg);
    ExecutionError executionError(const std::string& whatArg) const;

    const Program *program;
    ProgramState state;
    std::unique_ptr<Blueprint> output;
};

#endif // RUNNINGPROGRAM_H
