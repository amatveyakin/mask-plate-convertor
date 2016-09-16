#include "blueprint.h"
#include "programcommands.h"
#include "runningprogram.h"


const int mainRoutineIndex = -1;
const int maxRecursionDepth = 16;  // 5 by specification
const int startingLineWidth = 100;


ExecutionProblem::ExecutionProblem(TextRange commandTextRangeArg, const CallStack& callStackArg, const std::string& whatArg)
    : std::runtime_error(whatArg)
    , m_commandTextRange(commandTextRangeArg)
    , m_callStack(callStackArg)
{
}


RunningProgram::RunningProgram(const Program* programArg)
    : program(programArg)
    , output(new Blueprint)
{
}

void RunningProgram::addExecutionWarning(const std::string& whatArg)
{
    firstWarning = std::make_unique<ExecutionProblem>(executionError(whatArg));
}

ExecutionProblem RunningProgram::executionError(const std::string& whatArg) const
{
    return ExecutionProblem(state.currentCommand->textRange(), state.callStack, whatArg);
}
