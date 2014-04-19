#include "blueprint.h"
#include "programcommands.h"
#include "runningprogram.h"


const int mainRoutineIndex = -1;
const int maxRecursionDepth = 16;  // 5 by specification
const int startingLineWidth = 100;


ExecutionError::ExecutionError(TextRange commandTextRangeArg, const std::string& callStackDump, const std::string& whatArg)
    : std::runtime_error(whatArg + "\n\n" + callStackDump)
    , m_commandTextRange(commandTextRangeArg)
{
}


std::string CallStack::dump() const
{
    // TODO: Also print line numbers
    assert(!empty());
    std::string result;
    for (auto it = rbegin(); it != rend(); ++it) {
        if (it == rbegin())
            result += (it->routineIndex == mainRoutineIndex ? "В главной программе" : "В подпрограмме " + std::to_string(it->routineIndex)) + "\n";
        else
            result += (it->routineIndex == mainRoutineIndex ? "вызванной из главной программы" : "вызванной из подпрограммы " + std::to_string(it->routineIndex)) + "\n";
    }
    return result;
}


RunningProgram::RunningProgram(const Program* programArg)
    : program(programArg)
    , output(new Blueprint)
{
}

ExecutionError RunningProgram::executionError(const std::string& whatArg) const
{
    return ExecutionError(state.currentCommand->textRange(), state.callStack.dump(), whatArg);
}
