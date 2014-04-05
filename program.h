#ifndef PROGRAM_H
#define PROGRAM_H

#include <assert.h>

#include <memory>
#include <map>

class QPoint;

class Blueprint;
class ProgramCommand;
class Routine;
struct RunningProgram;


class Program
{
public:
    static const int mainRoutineIndex = -1;
    static const int maxRecursionDepth = 16;  // 5 by specification
    static const int startingLineWidth = 100;

    Program();
    ~Program();

    void pushBack(int routineIndex, std::unique_ptr<ProgramCommand> newCommand);

    std::unique_ptr<Blueprint> execute() const;

private:
    std::unique_ptr<Routine> m_mainRoutine;
    std::map<int, std::unique_ptr<Routine>> m_subroutines;

private:
    const Routine* mainRoutine() const;
    Routine* mainRoutine();
    const Routine* subroutine(int index) const;
    Routine* subroutine(int index);

private:
    friend class ProgramCommand;
    friend class CallSubroutineCommand;
};


struct Arguments
{
    void set(int index, int value)      { m_data[index] = value; }
    bool isSet(int index) const         { return m_data.count(index); }
    int get(int index) const            { auto it = m_data.find(index);  assert(it != m_data.end());  return it->second; }
private:
    std::map<int, int> m_data;
};

class Number
{
public:
    Number() : Number(Literal, 0) {}
    static Number literal(int value)    { return {Literal,  value}; }
    static Number variable(int value)   { return {Variable, value}; }
    int value(const Arguments& arguments) const;
private:
    enum Type { Literal, Variable };
private:
    Type m_type;
    int m_data;
private:
    Number(Type type, int data) : m_type(type), m_data(data) {}
};

struct Movement
{
    Number x, y;
    QPoint value(const Arguments& arguments);
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
