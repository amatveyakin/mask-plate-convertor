#include "forwardmapping.h"
#include "programbasic.h"


void ForwardMapping::addInfo(int line, int routineIndex)
{
    m_lineInfo.emplace(line, LineInfo{routineIndex});
}

void ForwardMapping::addMovement(int line, QPoint from, QPoint to)
{
    const QPoint movementValue = to - from;
    const auto it = m_lineMovements.find(line);
    if (it == m_lineMovements.end()) {
        m_lineMovements.emplace(line, LineMovement{false, movementValue});
    }
    else {
        LineMovement& lineMovement = it->second;
        if (lineMovement.movementValue != movementValue)
            lineMovement.isMovementAmbiguous = true;
    }
}

void ForwardMapping::addSegment(const CallStack& backtrace, SegmentId segment)
{
    for (const Call& call : backtrace)
        m_lineSegments.emplace(call.inputPosition.line, segment);
}

void ForwardMapping::lineIntervalMovement(int first, int last, bool& ok, QPoint& movement) const
{
    assert(first <= last);
    ok = false;
    movement = {};
    const auto routineFirstIt = m_lineInfo.find(first);
    const auto routineLastIt = m_lineInfo.find(last);
    if (routineFirstIt == m_lineInfo.end() || routineLastIt == m_lineInfo.end())
        return;
    if (routineFirstIt->second.routineIndex != routineLastIt->second.routineIndex)
        return;
    const auto movementBeginIt = m_lineMovements.lower_bound(first);
    const auto movementEndIt = m_lineMovements.upper_bound(last);
    for (auto it = movementBeginIt; it != movementEndIt; ++it) {
        const LineMovement& lineMovement = it->second;
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
