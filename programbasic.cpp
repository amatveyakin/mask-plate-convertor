#include <string>

#include "program.h"
#include "programbasic.h"
#include "runningprogram.h"


ExecutionError::ExecutionError(const std::string& callStackDump, const std::string& whatArg)
    : std::runtime_error(whatArg + "\n\n" + callStackDump) {}


Arguments Arguments::united(const Arguments& newArguments) const
{
    Arguments result(newArguments);
    result.m_data.insert(m_data.begin(), m_data.end());
    return result;
}


int Number::value(const Arguments& arguments, const RunningProgram& /*instance*/) const
{
    switch (m_type) {
    case Literal:
        return m_data * m_mult;
    case Variable:
        if (!arguments.isSet(m_data))
            return 0; // TODO FIXME // throw instance.executionError("Параметр " + std::to_string(m_data) + " использован, но не задан.");
        return arguments.get(m_data) * m_mult;
    }
    assert(false);
}


QPoint Movement::value(const Arguments& arguments, const RunningProgram& instance)
{
    return QPoint(x.value(arguments, instance), y.value(arguments, instance));
}
