#ifndef BLUEPRINT_H
#define BLUEPRINT_H

#include <vector>

#include <QPolygon>

#include "blueprintbasic.h"
#include "programbasic.h"


/*!
 * \brief Polygon with specific width and inverse search information
 */
struct Element
{
    Element() : width(-1) {}

    QPolygon polygon;
    int width;
    std::vector<CallStack> segmentBacktraces;
};

/*!
 * \brief Drawing consisting of several polygons of a certain width
 */
class Blueprint
{
public:
    static BlueprintPtr makePtr();

    Blueprint();
    ~Blueprint();

    const std::vector<Element>& elements() const    { return m_elements; }
    QRect boundingRect() const                      { return m_boundingRect; }
    bool isSegmentValid(SegmentId id) const;

    void appendLine(QPoint from, QPoint to, int width, const CallStack& backtrace);
    void finishElement();
    void preProcess();
    void postProcess();

private:
    std::vector<Element> m_elements;
    QRect m_boundingRect;
};

#endif // BLUEPRINT_H
