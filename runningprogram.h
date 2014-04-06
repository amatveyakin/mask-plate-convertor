#ifndef RUNNINGPROGRAM_H
#define RUNNINGPROGRAM_H

#include <memory>
#include <vector>

#include "programbasic.h"

class Blueprint;
class Program;


struct Call
{
    Call(int routineIndexArg, const Arguments &argumentsArg) : routineIndex(routineIndexArg), arguments(argumentsArg) {}

    int routineIndex;
    const Arguments &arguments;
};


class CallStack : public std::vector<Call>
{
public:
    std::string dump() const;
};


struct ProgramState
{
    ProgramState() : laserEnabled(false), lineWidth(startingLineWidth) {}

    CallStack callStack;
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
