#ifndef RUNNINGPROGRAM_H
#define RUNNINGPROGRAM_H

#include <memory>

#include "programbasic.h"

class Blueprint;
class Program;


struct Call
{
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
    CallStack callStack;
    bool laserEnabled   = false;
    int lineWidth       = startingLineWidth;
    QPoint position     = {0, 0};
};


struct RunningProgram
{
    RunningProgram(const Program *programArg);
    ExecutionError executionError(const std::string& whatArg) const;

    const Program *program = nullptr;
    ProgramState state;
    std::unique_ptr<Blueprint> output;
};

#endif // RUNNINGPROGRAM_H
