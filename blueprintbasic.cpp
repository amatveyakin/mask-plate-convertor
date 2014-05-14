#include "blueprint.h"
#include "blueprintbasic.h"


const Blueprint BlueprintPtr::defaultBlueprint;

BlueprintPtr::BlueprintPtr(Blueprint* ptr)
    : m_data(ptr)
{
}

BlueprintPtr::~BlueprintPtr()
{
}

void BlueprintPtr::reset(Blueprint* ptr)
{
    Blueprint* oldPtr = m_data.get();
    m_data.reset(ptr);
    emitChangedSignals(oldPtr);
}

BlueprintPtr& BlueprintPtr::operator=(BlueprintPtr&& rhs)
{
    Blueprint* oldPtr = m_data.get();
    m_data = rhs.m_data;
    emitChangedSignals(oldPtr);
    return *this;
}

BlueprintPtr& BlueprintPtr::operator=(const BlueprintPtr& rhs)
{
    Blueprint* oldPtr = m_data.get();
    m_data = rhs.m_data;
    emitChangedSignals(oldPtr);
    return *this;
}

void BlueprintPtr::emitChangedSignals(Blueprint* oldPtr)
{
    if (m_data.get() != oldPtr) {
        bool wasValid = bool(oldPtr);
        if (wasValid != isValid()) {
            emit isValidChanged(isValid());
            emit isNullChanged(isNull());
        }
        emit pointerChanged();
    }
}
