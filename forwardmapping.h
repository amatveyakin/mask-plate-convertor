#ifndef FORWARDMAPPING_H
#define FORWARDMAPPING_H

#include <map>
#include <set>

#include <QPoint>

#include "blueprintbasic.h"

class CallStack;


/*!
 * \brief Mapping from source line numbers to blueprint elements
 */
class ForwardMapping
{
public:
    void addMovement(QPoint from, QPoint to, int routineIndex, int line);
    void addSegment(SegmentId segment, const CallStack& backtrace);

    void lineIntervalMovement(int first, int last, bool& ok, QPoint& movement) const;
    std::vector<SegmentId> lineIntervalSegments(int first, int last) const;

private:
    struct LineMovement
    {
        int routineIndex;
        bool isMovementAmbiguous;
        QPoint movementValue;
    };

    /*!
     * Information about lines that produced movement.
     *
     * A movement is unambiguous if each time the line was executed it produced the same movement, i.e.
     * movement produced by argument dependent subroutine is still unambiguous it the routine was always
     * called with the same arguments.
     */
    std::map<int, LineMovement> m_lineMovements;

    /*!
     * Segments produced by this line (directly or indirectly).
     */
    std::multimap<int, SegmentId> m_lineSegments;
};

#endif // FORWARDMAPPING_H
