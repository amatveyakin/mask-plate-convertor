#include <assert.h>

#include "blueprint.h"


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

bool Blueprint::isSegmentValid(SegmentId id) const
{
    return    0 <= id.element && id.element < int(elements().size())
           && 0 <= id.segment && id.segment < elements()[id.element].polygon.size() - 1;
}

void Blueprint::appendLine(bool laserOn, QPoint from, QPoint to, int width, const CallStack& backtrace)
{
    if (m_elements.empty() || m_elements.back().laserOn != laserOn)
        m_elements.emplace_back();
    Element& curElement = m_elements.back();
    if (curElement.polygon.empty()) {
        curElement.laserOn = laserOn;
        curElement.polygon << from << to;
        curElement.width = width;
        curElement.segmentBacktraces.push_back(backtrace);
    }
    else {
        assert(curElement.laserOn == laserOn);
        assert(curElement.polygon.back() == from);
        if (laserOn)
            assert(curElement.width == width);
        curElement.polygon << to;
        curElement.segmentBacktraces.push_back(backtrace);
    }
    SegmentId lastSegment(m_elements.size() - 1, curElement.polygon.size() - 2);
    assert(isSegmentValid(lastSegment));
    m_forwardMapping.addSegment(backtrace, lastSegment);
    m_stopPoint = to;
}

void Blueprint::finishElement() {
    if (!m_elements.empty() && !m_elements.back().polygon.empty())
        m_elements.emplace_back();
}

void Blueprint::preProcess(const ForwardMapping& forwardMapping)
{
    m_forwardMapping = forwardMapping;
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

void Blueprint::resetForwardMapping()
{
    m_forwardMapping = {};
}
