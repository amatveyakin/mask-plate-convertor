#ifndef BLUEPRINT_H
#define BLUEPRINT_H

#include <vector>

#include <QPolygon>

#include "blueprintbasic.h"
#include "forwardmapping.h"
#include "programbasic.h"


/*!
 * \brief Polygon with specific width and inverse search information
 */
struct Element
{
    bool laserOn = false;
    QPolygon polygon;
    int width = -1;
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
    const ForwardMapping& forwardMapping() const    { return m_forwardMapping; }
    ForwardMapping& forwardMapping()                { return m_forwardMapping; }
    QPoint stopPoint() const                        { return m_stopPoint; }

    void appendLine(bool laserOn, QPoint from, QPoint to, int width, const CallStack& backtrace);
    void finishElement();
    void preProcess(const ForwardMapping& forwardMapping);
    void postProcess();
    void resetForwardMapping();

private:
    std::vector<Element> m_elements;
    QRect m_boundingRect;
    ForwardMapping m_forwardMapping;
    QPoint m_stopPoint;
};

#endif // BLUEPRINT_H
