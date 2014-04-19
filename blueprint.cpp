#include <assert.h>

#include "blueprint.h"


Blueprint::Blueprint()
{
    m_elements.emplace_back();
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

static QRect elementBoundingRect(const Element& element)
{
    int bonus = (element.width + 1) / 2;
    return element.polygon.boundingRect().adjusted(-bonus, -bonus, bonus, bonus);
}

void Blueprint::postProcess()
{
    assert(!m_elements.empty());
    if (m_elements.back().polygon.empty())
        m_elements.pop_back();
    m_boundingRect = elementBoundingRect(m_elements.front());
    for (const Element& element : m_elements)
        m_boundingRect |= elementBoundingRect(element);
}
