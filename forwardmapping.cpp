#include "forwardmapping.h"
#include "programbasic.h"


void ForwardMapping::addMovement(bool argumentDependent, QPoint from, QPoint to, int routineIndex, int line)
{
    if (argumentDependent) {
        m_breakerLines.insert(line);
    }
    else {
        m_lineBeginPos.emplace(line, PositionInfo{from, routineIndex});
        m_lineEndPos.emplace(line, PositionInfo{to, routineIndex});
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
    auto beginIt = m_lineBeginPos.lower_bound(first);
    auto endIt = m_lineEndPos.upper_bound(last);
    if (beginIt == m_lineBeginPos.end())
        return;
    if (endIt == m_lineEndPos.begin())
        return;
    endIt--;
    if (   m_breakerLines.lower_bound(first) == m_breakerLines.upper_bound(last)
        && beginIt->first <= endIt->first
        && beginIt->second.routineIndex == endIt->second.routineIndex) {
        ok = true;
        movement = endIt->second.position - beginIt->second.position;
    }
}

std::vector<SegmentId> ForwardMapping::lineIntervalSegments(int first, int last) const
{
    assert(first <= last);
    std::vector<SegmentId> segments;
    auto beginIt = m_lineSegments.lower_bound(first);
    auto endIt = m_lineSegments.upper_bound(last);
    for (auto it = beginIt; it != endIt; ++it)
        segments.push_back(it->second);
    return segments;
}
