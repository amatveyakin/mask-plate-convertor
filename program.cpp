#include <stack>
#include <vector>

#include <QPoint>

#include "blueprint.h"
#include "program.h"


class ExecutionError : public std::runtime_error
{
public:
    ExecutionError(const std::string& what_arg) : std::runtime_error(what_arg) {}  // TODO: Print callstack
};


int Number::value(const Arguments& arguments) const
{
    switch (m_type) {
    case Literal:
        return m_data;
    case Variable:
        if (!arguments.isSet(m_data))
            throw ExecutionError("Параметр " + std::to_string(m_data) + " использован, но не задан.");  // TODO: Context
        return arguments.get(m_data);
    }
    assert(false);
}


QPoint Movement::value(const Arguments& arguments)
{
    return {x.value(arguments), y.value(arguments)};
}


struct Call
{
    int routineIndex;
    const Arguments &arguments;
};

struct ProgramState
{
    std::stack<Call> callStack;
    bool laserEnabled   = false;
    int lineWidth       = Program::startingLineWidth;
    QPoint position     = {0, 0};
};

struct RunningProgram
{
    RunningProgram(const Program *programArg)
        : program(programArg)
        , output(new Blueprint)
    {
    }

    const Program *program = nullptr;
    ProgramState state;
    std::unique_ptr<Blueprint> output;
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


Routine *nonnullRoutine(std::unique_ptr<Routine>& routine, int index)
{
    if (!routine)
        routine.reset(new Routine(index));
    return routine.get();
}

Program::Program()
{
}

Program::~Program()
{
}

void Program::pushBack(int routineIndex, std::unique_ptr<ProgramCommand> newCommand)
{
    if (routineIndex == mainRoutineIndex)
        mainRoutine()->pushBack(std::move(newCommand));
    else
        subroutine(routineIndex)->pushBack(std::move(newCommand));
}

std::unique_ptr<Blueprint> Program::execute() const
{
    RunningProgram instance(this);
    mainRoutine()->execute(instance, {});
    return std::move(instance.output);
}

const Routine* Program::mainRoutine() const
{
    return m_mainRoutine.get();
}

Routine* Program::mainRoutine()
{
    return nonnullRoutine(m_mainRoutine, mainRoutineIndex);
}

const Routine* Program::subroutine(int index) const
{
    auto it = m_subroutines.find(index);
    return it == m_subroutines.end() ? nullptr : it->second.get();
}

Routine* Program::subroutine(int index)
{
    return nonnullRoutine(m_subroutines[index], index);
}


EnableLaserCommand::EnableLaserCommand()
{
}

void EnableLaserCommand::execute(RunningProgram& instance)
{
    instance.state.laserEnabled = true;
}


DisableLaserCommand::DisableLaserCommand()
{
}

void DisableLaserCommand::execute(RunningProgram& instance)
{
    instance.state.laserEnabled = false;
    instance.output->finishElement();
}


SetLineWidthCommand::SetLineWidthCommand(int newWidth)
    : m_newWidth(newWidth)
{
}

void SetLineWidthCommand::execute(RunningProgram& instance)
{
    if (instance.state.laserEnabled)
        throw ExecutionError("Попытка изменения ширины зазора при включённом лазере");
    instance.state.lineWidth = m_newWidth;
}


MoveToCommand::MoveToCommand(Movement movement)
    : m_movement(movement)
{
}

void MoveToCommand::execute(RunningProgram& instance)
{
    QPoint oldPosition = instance.state.position;
    instance.state.position += m_movement.value(instance.state.callStack.top().arguments);
    if (instance.state.laserEnabled)
        instance.output->appendLineTo(oldPosition, instance.state.position, instance.state.lineWidth);
}


CallSubroutineCommand::CallSubroutineCommand(int subroutineIndex, int repeatCount, const Arguments& arguments)
    : m_subroutineIndex(subroutineIndex)
    , m_repeatCount(repeatCount)
    , m_arguments(arguments)
{
}

void CallSubroutineCommand::execute(RunningProgram& instance)
{
    // TODO: in case of error:  print stack;  always print 2 digits for index
    const Routine* subroutine = instance.program->subroutine(m_subroutineIndex);
    if (!subroutine)
        throw ExecutionError("Подпрограмма " + std::to_string(m_subroutineIndex) + "не существует");
    if (instance.state.callStack.size() >= Program::maxRecursionDepth)  // TODO: +-1 ?
        throw ExecutionError("Превышена максимальная глубина рекурсии при попытке вызвать подпрограмму " + std::to_string(m_subroutineIndex));
    for (int i = 0; i < m_repeatCount; ++i)
        subroutine->execute(instance, m_arguments);
}
