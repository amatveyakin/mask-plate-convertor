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
    void addMovement(bool argumentDependent, QPoint from, QPoint to, int routineIndex, int line);
    void addSegment(SegmentId segment, const CallStack& backtrace);

    void lineIntervalMovement(int first, int last, bool& ok, QPoint& movement) const;
    std::vector<SegmentId> lineIntervalSegments(int first, int last) const;

private:
    struct PositionInfo
    {
        QPoint position;
        int routineIndex;
    };

    /*!
     * Laser position at the beginning of line.
     * Some unknown (but well-defined) position for subroutines.
     */
    std::map<int, PositionInfo> m_lineBeginPos;

    /*!
     * Laser position at the end of line.
     * Some unknown (but well-defined) position for subroutines.
     */
    std::map<int, PositionInfo> m_lineEndPos;

    /*!
     * (lineEndPos[j] - lineEndPos[i]) is the correct movement between lines i..j
     * if m_breakerLines doesn't contain lines i..j.
     */
    std::set<int> m_breakerLines;

    /*!
     * Segments produced by this line (directly or indirectly).
     */
    std::multimap<int, SegmentId> m_lineSegments;
};

#endif // FORWARDMAPPING_H
