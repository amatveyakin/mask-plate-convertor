#include "blueprint.h"
#include "program.h"
#include "programcommands.h"
#include "runningprogram.h"


void ProgramCommand::execute(RunningProgram& instance)
{
    assert(!instance.state.callStack.empty());
    instance.state.callStack.back().inputPosition = m_textRange.begin;
    doExecute(instance);
}

int ProgramCommand::textLine() const
{
    int line = m_textRange.begin.line;
    assert(line == m_textRange.end.line);
    return line;
}


EnableLaserCommand::EnableLaserCommand()
{
}

void EnableLaserCommand::doExecute(RunningProgram& instance)
{
    instance.state.laserEnabled = true;
}


DisableLaserCommand::DisableLaserCommand()
{
}

void DisableLaserCommand::doExecute(RunningProgram& instance)
{
    instance.state.laserEnabled = false;
    instance.output->finishElement();
}


SetLineWidthCommand::SetLineWidthCommand(int newWidth)
    : m_newWidth(newWidth)
{
}

void SetLineWidthCommand::doExecute(RunningProgram& instance)
{
    if (instance.state.laserEnabled)
        throw instance.executionError("Попытка изменения ширины зазора при включённом лазере");
    instance.state.lineWidth = m_newWidth;
}


MoveToCommand::MoveToCommand(Movement movement)
    : m_movement(movement)
{
}

void MoveToCommand::doExecute(RunningProgram& instance)
{
    int currentRoutineIndex = instance.state.callStack.currentRoutine();
    QPoint oldPosition = instance.state.position;
    instance.state.position += m_movement.value(instance.state.arguments, instance);
    QPoint newPosition = instance.state.position;
    if (instance.state.laserEnabled)
        instance.output->appendLine(oldPosition, newPosition, instance.state.lineWidth, instance.state.callStack);
    instance.output->forwardMapping().addMovement(m_movement.argumentDependent(), oldPosition, newPosition, currentRoutineIndex, textLine());
    instance.output->setStopPoint(instance.state.position);
}


CallSubroutineCommand::CallSubroutineCommand(int subroutineIndex, int repeatCount, const Arguments& arguments)
    : m_subroutineIndex(subroutineIndex)
    , m_repeatCount(repeatCount)
    , m_arguments(arguments)
{
}

void CallSubroutineCommand::doExecute(RunningProgram& instance)
{
    int currentRoutineIndex = instance.state.callStack.currentRoutine();
    // TODO: in case of error: always print 2 digits for index
    const Routine* subroutine = instance.program->routine(m_subroutineIndex);
    if (!subroutine)
        throw instance.executionError("Подпрограмма " + std::to_string(m_subroutineIndex) + " не существует");
    if ((int)instance.state.callStack.size() >= maxRecursionDepth)  // TODO: +-1 ?
        throw instance.executionError("Превышена максимальная глубина рекурсии при попытке вызвать подпрограмму " + std::to_string(m_subroutineIndex));
    QPoint oldPosition = instance.state.position;
    for (int i = 0; i < m_repeatCount; ++i)
        subroutine->execute(instance, m_arguments);
    QPoint newPosition = instance.state.position;
    bool argumentDependent = (currentRoutineIndex != mainRoutineIndex);  // TODO: Also recognize subroutines calling other subroutines without arguments
    instance.output->forwardMapping().addMovement(argumentDependent, oldPosition, newPosition, currentRoutineIndex, textLine());
}
