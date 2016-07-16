#include "forwardmapping.h"
#include "programbasic.h"


void ForwardMapping::addMovement(QPoint from, QPoint to, int routineIndex, int line)
{
    const QPoint movementValue = to - from;
    const auto it = m_lineMovements.find(line);
    if (it == m_lineMovements.end()) {
        m_lineMovements.emplace(line, LineMovement{routineIndex, false, movementValue});
    }
    else {
        LineMovement& lineMovement = it->second;
        assert(lineMovement.routineIndex == routineIndex);
        if (lineMovement.movementValue != movementValue)
            lineMovement.isMovementAmbiguous = true;
    }
}

void ForwardMapping::addSegment(SegmentId segment, const CallStack& backtrace)
{
    for (const Call& call : backtrace)
        m_lineSegments.emplace(call.inputPosition.line, segment);
}

void ForwardMapping::lineIntervalMovement(int first, int last, bool& ok, QPoint& movement) const
{
    assert(first <= last);
    ok = false;
    movement = {};
    const auto beginIt = m_lineMovements.lower_bound(first);
    const auto endIt = m_lineMovements.upper_bound(last);
    if (beginIt == m_lineMovements.end())
        return;
    const int routineIndex = beginIt->second.routineIndex;
    for (auto it = beginIt; it != endIt; ++it) {
        const LineMovement& lineMovement = it->second;
        if (lineMovement.routineIndex != routineIndex)
            return;
        if (lineMovement.isMovementAmbiguous)
            return;
        movement += lineMovement.movementValue;
    }
    ok = true;
}

std::vector<SegmentId> ForwardMapping::lineIntervalSegments(int first, int last) const
{
    assert(first <= last);
    std::vector<SegmentId> segments;
    const auto beginIt = m_lineSegments.lower_bound(first);
    const auto endIt = m_lineSegments.upper_bound(last);
    for (auto it = beginIt; it != endIt; ++it)
        segments.push_back(it->second);
    return segments;
}
