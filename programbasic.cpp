#include <string>

#include "program.h"
#include "programbasic.h"
#include "runningprogram.h"


Arguments Arguments::replaced(const Arguments& newArguments) const
{
    Arguments result(newArguments);
    result.m_data.insert(m_data.begin(), m_data.end());
    return result;
}


int Number::value(const Arguments& arguments, const RunningProgram& instance) const
{
    switch (m_type) {
    case Literal:
        return m_data * m_mult;
    case Variable:
        if (!arguments.isSet(m_data))
            throw instance.executionError("Параметр " + std::to_string(m_data) + " использован, но не задан.");
        return arguments.get(m_data) * m_mult;
    }
    assert(false);
}


QPoint Movement::value(const Arguments& arguments, const RunningProgram& instance)
{
    return QPoint(x.value(arguments, instance), y.value(arguments, instance));
}

bool Movement::argumentDependent() const
{
    return x.argumentDependent() || y.argumentDependent();
}


int CallStack::currentRoutine() const
{
    assert(!empty());
    return back().routineIndex;
}
