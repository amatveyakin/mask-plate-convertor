#include <assert.h>

#include "blueprint.h"


//==============================================================================================================================================================
// Blueprint

BlueprintPtr Blueprint::makePtr()
{
    return BlueprintPtr(new Blueprint);
}

Blueprint::Blueprint()
    : m_boundingRect(0, 0, 1, 1)
{
}

Blueprint::~Blueprint()
{
}

void Blueprint::appendLine(QPoint from, QPoint to, int width)
{
    assert(!m_elements.empty());
    Element& curElement = m_elements.back();
    if (curElement.polygon.empty()) {
        curElement.polygon << from << to;
        curElement.width = width;
    }
    else {
        assert(curElement.polygon.back() == from);
        assert(curElement.width == width);
        curElement.polygon << to;
    }
}

void Blueprint::finishElement()
{
    assert(!m_elements.empty());
    if (!m_elements.back().polygon.empty())
        m_elements.emplace_back();
}

void Blueprint::preProcess()
{
    m_elements.emplace_back();
}

static QRect elementBoundingRect(const Element& element)
{
    int bonus = (element.width + 1) / 2;
    return element.polygon.boundingRect().adjusted(-bonus, -bonus, bonus, bonus);
}

static void ensureMinSize(QRect& rect, int size)
{
    if (rect.width() < size)
        rect.setWidth(size);
    if (rect.height() < size)
        rect.setHeight(size);
}

void Blueprint::postProcess()
{
    assert(!m_elements.empty());
    if (m_elements.back().polygon.empty())
        m_elements.pop_back();
    if (!m_elements.empty()) {
        m_boundingRect = elementBoundingRect(m_elements.front());
        for (const Element& element : m_elements)
            m_boundingRect |= elementBoundingRect(element);
        ensureMinSize(m_boundingRect, 1);
    }
}


//==============================================================================================================================================================
// BlueprintPtr

const Blueprint BlueprintPtr::defaultBlueprint;

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
