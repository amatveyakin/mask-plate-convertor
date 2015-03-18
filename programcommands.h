#ifndef PROGRAMCOMMANDS_H
#define PROGRAMCOMMANDS_H

#include "programbasic.h"


class ProgramCommand
{
public:
    virtual ~ProgramCommand() {}

    void execute(RunningProgram& instance);

    void setTextRange(TextRange textRangeArg)   { m_textRange = textRangeArg; }
    TextRange textRange() const                 { return m_textRange; }
    int textLine() const;

protected:
    virtual void doExecute(RunningProgram& instance) = 0;

private:
    TextRange m_textRange;
};


class EnableLaserCommand : public ProgramCommand
{
public:
    EnableLaserCommand();
protected:
    virtual void doExecute(RunningProgram& instance) override;
};


class DisableLaserCommand : public ProgramCommand
{
public:
    DisableLaserCommand();
protected:
    virtual void doExecute(RunningProgram& instance) override;
};


class SetLineWidthCommand : public ProgramCommand
{
public:
    SetLineWidthCommand(int newWidth);
protected:
    virtual void doExecute(RunningProgram& instance) override;
private:
    int m_newWidth;
};


class MoveToCommand : public ProgramCommand
{
public:
    MoveToCommand(Movement movement);
protected:
    virtual void doExecute(RunningProgram& instance) override;
private:
    Movement m_movement;
};


class CallSubroutineCommand : public ProgramCommand
{
public:
    CallSubroutineCommand(int subroutineIndex, int repeatCount, const Arguments &arguments);
protected:
    virtual void doExecute(RunningProgram& instance) override;
private:
    int m_subroutineIndex;
    int m_repeatCount;
    Arguments m_arguments;
};

#endif // PROGRAMCOMMANDS_H
