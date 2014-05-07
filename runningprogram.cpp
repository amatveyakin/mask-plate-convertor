#include "blueprint.h"
#include "programcommands.h"
#include "runningprogram.h"


const int mainRoutineIndex = -1;
const int maxRecursionDepth = 16;  // 5 by specification
const int startingLineWidth = 100;


ExecutionError::ExecutionError(TextRange commandTextRangeArg, const CallStack& callStackArg, const std::string& whatArg)
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

ExecutionError RunningProgram::executionError(const std::string& whatArg) const
{
    return ExecutionError(state.currentCommand->textRange(), state.callStack, whatArg);
}
