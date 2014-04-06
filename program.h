#ifndef PROGRAM_H
#define PROGRAM_H

#include <assert.h>

#include <memory>
#include <map>

#include "programbasic.h"

class QPoint;

class Blueprint;
class ProgramCommand;
class Routine;
struct RunningProgram;


class Program
{
public:
    static const int mainRoutineIndex;
    static const int maxRecursionDepth;
    static const int startingLineWidth;

    static ExecutionError executionError(const RunningProgram& instance, const std::string& whatArg);

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


class ProgramCommand
{
public:
    virtual ~ProgramCommand() {}
    virtual void execute(RunningProgram& instance) = 0;
};

class EnableLaserCommand : public ProgramCommand
{
public:
    EnableLaserCommand();
    virtual void execute(RunningProgram& instance) override;
};

class DisableLaserCommand : public ProgramCommand
{
public:
    DisableLaserCommand();
    virtual void execute(RunningProgram& instance) override;
};

class SetLineWidthCommand : public ProgramCommand
{
public:
    SetLineWidthCommand(int newWidth);
    virtual void execute(RunningProgram& instance) override;
private:
    int m_newWidth;
};

class MoveToCommand : public ProgramCommand
{
public:
    MoveToCommand(Movement movement);
    virtual void execute(RunningProgram& instance) override;
private:
    Movement m_movement;
};

class CallSubroutineCommand : public ProgramCommand
{
public:
    CallSubroutineCommand(int subroutineIndex, int repeatCount, const Arguments &arguments);
    virtual void execute(RunningProgram& instance) override;
private:
    int m_subroutineIndex;
    int m_repeatCount;
    Arguments m_arguments;
};

#endif // PROGRAM_H
