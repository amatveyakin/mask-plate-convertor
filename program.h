#ifndef PROGRAM_H
#define PROGRAM_H

#include <assert.h>

#include <memory>
#include <map>
#include <vector>

#include "programbasic.h"

class QPoint;

class Blueprint;
class ProgramCommand;
class Routine;
struct RunningProgram;


class Program
{
public:
    Program();
    ~Program();

    void pushBack(int routineIndex, std::unique_ptr<ProgramCommand> newCommand);

    std::unique_ptr<Blueprint> execute() const;

private:
    std::map<int, std::unique_ptr<Routine>> m_routines;

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
    int m_index;
    std::vector<std::unique_ptr<ProgramCommand>> m_commands;
};

#endif // PROGRAM_H
