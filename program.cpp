#include <stack>
#include <vector>

#include <QPoint>

#include "blueprint.h"
#include "program.h"


class ProgramCommand;

static const int mainRoutineIndex = -1;
static const int maxRecursionDepth = 5;


class ExecutionError : public std::runtime_error
{
public:
    ExecutionError(const std::string& what_arg) : std::runtime_error(what_arg) {}
};

struct Arguments
{
    void set(int index, int value)      { m_data[index] = value; }
    bool isSet(int index) const         { return m_data.count(index); }
    int get(int index) const            { auto it = m_data.find(index);  if (it == m_data.end()) abort();  return it->second; }
private:
    std::map<int, int> m_data;
};

class Number
{
public:
    static Number literal(int value)    { return {Literal,  value}; }
    static Number variable(int value)   { return {Variable, value}; }
    int value(const Arguments& arguments) const
    {
        switch (m_type) {
        case Literal:
            return m_data;
        case Variable:
            if (!arguments.isSet(m_data))
                throw ExecutionError("Параметр " + std::to_string(m_data) + " использован, но не задан.");  // TODO: Context
            return arguments.get(m_data);
        }
        abort();
    }
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
    QPoint value(const Arguments& arguments)
    {
        return {x.value(arguments), y.value(arguments)};
    }
};

struct Call
{
    int routineIndex;
    const Arguments &arguments;
};

struct ProgramState
{
    std::stack<Call> callStack;
    bool laserEnabled   = false;
    int lineWidth       = 100;
    QPoint position     = {0, 0};
};

struct RunningProgram
{
    RunningProgram(const Program *program)
        : program(program)
    {
    }

    const Program *program = nullptr;
    ProgramState state;
    Blueprint output;
};

class Routine
{
public:
    Routine(int index) : m_index(index) {}
    ~Routine() {}

    void pushBack(std::unique_ptr<ProgramCommand> newCommand);
    void execute(RunningProgram& instance, const Arguments &arguments) const;

private:
    int m_index;
    std::vector<std::unique_ptr<ProgramCommand>> m_commands;
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
    EnableLaserCommand()
    {
    }
    virtual void execute(RunningProgram& instance) override
    {
        instance.state.laserEnabled = true;
    }
};

class DisableLaserCommand : public ProgramCommand
{
public:
    DisableLaserCommand()
    {
    }
    virtual void execute(RunningProgram& instance) override
    {
        instance.state.laserEnabled = false;
    }
};

class SetLineWidthCommand : public ProgramCommand
{
public:
    SetLineWidthCommand(int newWidth)
        : m_newWidth(newWidth)
    {
    }
    virtual void execute(RunningProgram& instance) override
    {
        instance.state.lineWidth = m_newWidth;
    }
private:
    int m_newWidth;
};

class MoveToCommand : public ProgramCommand
{
public:
    MoveToCommand(Movement movement)
        : m_movement(movement)
    {
    }
    virtual void execute(RunningProgram& instance) override
    {
        instance.state.position += m_movement.value(instance.state.callStack.top().arguments);
    }
private:
    Movement m_movement;
};

class CallSubroutineCommand : public ProgramCommand
{
public:
    CallSubroutineCommand(int programIndex, int repeatCount, const Arguments &arguments)
        : m_programIndex(programIndex)
        , m_repeatCount(repeatCount)
        , m_arguments(arguments)
    {
    }
    virtual void execute(RunningProgram& instance) override
    {
        // TODO: in case of error:  print stack;  always print 2 digits for index
        const Routine* subroutine = instance.program->subroutine(m_programIndex);
        if (!subroutine)
            throw ExecutionError("Ошибка вызова подпрограммы: подпрограмма " + std::to_string(m_programIndex) + "не существует");
        if (instance.state.callStack.size() >= maxRecursionDepth)  // TODO: +-1 ?
            throw ExecutionError("Ошибка вызова подпрограммы: превышена максимальная глубина рекурсии при попытке вызвать подпрограмму " + std::to_string(m_programIndex));
        for (int i = 0; i < m_repeatCount; ++i)
            subroutine->execute(instance, m_arguments);
    }
private:
    int m_programIndex;
    int m_repeatCount;
    Arguments m_arguments;
};


void Routine::pushBack(std::unique_ptr<ProgramCommand> newCommand)
{
    m_commands.push_back(std::move(newCommand));
}

void Routine::execute(RunningProgram& instance, const Arguments &arguments) const
{
    instance.state.callStack.push({m_index, arguments});
    for (auto&& command : m_commands)
        command->execute(instance);
}


Program::Program()
{
}

Program::~Program()
{
}

const Routine* Program::mainRoutine() const
{
    return m_mainRoutine.get();
}

const Routine* Program::subroutine(int index) const
{
    auto it = m_subroutines.find(index);
    return it == m_subroutines.end() ? nullptr : it->second.get();
}

Blueprint Program::execute() const
{
    RunningProgram instance(this);
    mainRoutine()->execute(instance, {});
    return instance.output;
}
