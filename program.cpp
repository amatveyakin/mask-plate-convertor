#include <stack>
#include <vector>

#include <QPoint>

#include "blueprint.h"
#include "cpp_extensions.h"
#include "program.h"
#include "programcommands.h"
#include "runningprogram.h"


Routine::Routine(int index)
    : m_index(index)
{
}

Routine::~Routine()
{
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
