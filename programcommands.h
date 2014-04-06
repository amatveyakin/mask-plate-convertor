#ifndef PROGRAMCOMMANDS_H
#define PROGRAMCOMMANDS_H

#include "programbasic.h"


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

#endif // PROGRAMCOMMANDS_H
