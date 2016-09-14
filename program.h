#ifndef PROGRAM_H
#define PROGRAM_H

#include <assert.h>

#include <memory>
#include <map>
#include <vector>

#include "forwardmapping.h"
#include "programbasic.h"

class QPoint;

class Blueprint;
class ExecutionProblem;
class ProgramCommand;
class Routine;
struct RunningProgram;


struct ExecutionResult
{
    std::unique_ptr<Blueprint> blueprint;
    std::unique_ptr<ExecutionProblem> executionWarning;
};

class Program
{
public:
    Program();
    ~Program();

    const ForwardMapping& forwardMapping() const    { return m_forwardMapping; }
    ForwardMapping& forwardMapping()                { return m_forwardMapping; }

    bool hasRoutine(int index) const;

    void pushBack(int routineIndex, std::unique_ptr<ProgramCommand> newCommand);

    ExecutionResult execute() const;

private:
    std::map<int, std::unique_ptr<Routine>> m_routines;
    ForwardMapping m_forwardMapping;

private:
    const Routine* routine(int index) const;
    Routine* nonnullRoutine(int index);

private:
    friend class ProgramCommand;
    friend class CallSubroutineCommand;
};


class Routine
{
public:
    Routine(int index);
    ~Routine();

    void pushBack(std::unique_ptr<ProgramCommand> newCommand);
    void execute(RunningProgram& instance, const Arguments &arguments) const;

private:
    int m_index = mainRoutineIndex;
    std::vector<std::unique_ptr<ProgramCommand>> m_commands;
};

#endif // PROGRAM_H
