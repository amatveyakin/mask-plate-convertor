#include <stack>
#include <vector>

#include <QPoint>

#include "blueprint.h"
#include "cpp_extensions.h"
#include "program.h"


const int Program::mainRoutineIndex = -1;
const int Program::maxRecursionDepth = 16;  // 5 by specification
const int Program::startingLineWidth = 100;


class CallStack;

class ExecutionError : public std::runtime_error
{
public:
    ExecutionError(const CallStack& callStack, const std::string& what_arg);
};

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
    int lineWidth       = Program::startingLineWidth;
    QPoint position     = {0, 0};
};

struct RunningProgram
{
    RunningProgram(const Program *programArg)
        : program(programArg)
        , output(new Blueprint) {}
    ExecutionError executionError(const std::string& what_arg) const { return ExecutionError(state.callStack, what_arg); }

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


int Number::value(const Arguments& arguments, const RunningProgram& instance) const
{
    switch (m_type) {
    case Literal:
        return m_data * m_mult;
    case Variable:
        if (!arguments.isSet(m_data))
            throw instance.executionError("Параметр " + std::to_string(m_data) + " использован, но не задан.");  // TODO: Context
        return arguments.get(m_data) * m_mult;
    }
    assert(false);
}

QPoint Movement::value(const Arguments& arguments, const RunningProgram& instance)
{
    return {x.value(arguments, instance), y.value(arguments, instance)};
}

std::string CallStack::dump() const
{
    assert(!empty());
    std::string result;
    for (auto it = rbegin(); it != rend(); ++it) {
        if (it == rbegin())
            result += (it->routineIndex == Program::mainRoutineIndex ? "В главной программе" : "В подпрограмме " + std::to_string(it->routineIndex)) + "\n";
        else
            result += (it->routineIndex == Program::mainRoutineIndex ? "вызванной из главной программы" : "вызванной из подпрограммы " + std::to_string(it->routineIndex)) + "\n";
    }
    return result;
}

void Routine::pushBack(std::unique_ptr<ProgramCommand> newCommand)
{
    assert(newCommand);
    m_commands.push_back(std::move(newCommand));
}

void Routine::execute(RunningProgram& instance, const Arguments &arguments) const
{
    instance.state.callStack.push_back({m_index, arguments});
    for (auto&& command : m_commands)
        command->execute(instance);
    instance.state.callStack.pop_back();
}


Program::Program()
{
    m_routines.emplace(mainRoutineIndex, std::make_unique<Routine>(mainRoutineIndex));
}

Program::~Program()
{
}

void Program::pushBack(int routineIndex, std::unique_ptr<ProgramCommand> newCommand)
{
//    qDebug("@@ pushing to subroutine %d", routineIndex); // TODO: Delete
    nonnullRoutine(routineIndex)->pushBack(std::move(newCommand));
}

std::unique_ptr<Blueprint> Program::execute() const
{
    RunningProgram instance(this);
    routine(mainRoutineIndex)->execute(instance, {});
    return std::move(instance.output);
}

const Routine* Program::routine(int index) const
{
    auto it = m_routines.find(index);
    return it == m_routines.end() ? nullptr : it->second.get();
}

Routine* Program::nonnullRoutine(int index)
{
    auto it = m_routines.find(index);
    if (it == m_routines.end())
        it = m_routines.emplace(index, std::make_unique<Routine>(index)).first;
    return it->second.get();
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
        throw ExecutionError(instance.state.callStack, "Попытка изменения ширины зазора при включённом лазере");
    instance.state.lineWidth = m_newWidth;
}


MoveToCommand::MoveToCommand(Movement movement)
    : m_movement(movement)
{
}

void MoveToCommand::execute(RunningProgram& instance)
{
    QPoint oldPosition = instance.state.position;
    instance.state.position += m_movement.value(instance.state.callStack.back().arguments, instance);
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
    // TODO: in case of error: always print 2 digits for index
    const Routine* subroutine = instance.program->routine(m_subroutineIndex);
    if (!subroutine)
        throw instance.executionError("Подпрограмма " + std::to_string(m_subroutineIndex) + "не существует");
    if (instance.state.callStack.size() >= Program::maxRecursionDepth)  // TODO: +-1 ?
        throw instance.executionError("Превышена максимальная глубина рекурсии при попытке вызвать подпрограмму " + std::to_string(m_subroutineIndex));
    for (int i = 0; i < m_repeatCount; ++i)
        subroutine->execute(instance, m_arguments);
}


ExecutionError::ExecutionError(const CallStack& callStack, const std::string& what_arg)
    : std::runtime_error(what_arg + "\n\n" + callStack.dump()) {}
