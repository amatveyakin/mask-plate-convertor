#ifndef BLUEPRINTBASIC_H
#define BLUEPRINTBASIC_H

#include <assert.h>

#include <memory>

#include <QObject>

class Blueprint;


/*!
 * \brief Unique identifier of a segment in a blueprint
 */
struct SegmentId
{
    int element = -1;
    int segment = -1;

    static SegmentId invalid() { return {}; }

    SegmentId() {}
    SegmentId(int elementArg, int segmentArg) : element(elementArg), segment(segmentArg) {}

    bool operator==(const SegmentId& rhs) const { return std::tie(element, segment) == std::tie(rhs.element, rhs.segment); }
    bool operator!=(const SegmentId& rhs) const { return !(*this == rhs); }
};

/*!
 * \brief Blueprint shared pointer that can be always dereferenced
 */
class BlueprintPtr : public QObject
{
    Q_OBJECT

public:
    BlueprintPtr() {}
    explicit BlueprintPtr(Blueprint* ptr);
    ~BlueprintPtr();

    BlueprintPtr(const BlueprintPtr& rhs) : QObject(), m_data(rhs.m_data) {}
    BlueprintPtr(BlueprintPtr&& rhs) : QObject(), m_data(rhs.m_data) {}
    BlueprintPtr& operator=(const BlueprintPtr& rhs);
    BlueprintPtr& operator=(BlueprintPtr&& rhs);

    void reset(Blueprint* ptr);

    bool isValid() const                { return bool(m_data); }
    bool isNull() const                 { return !isValid(); }
    const Blueprint* get() const        { return m_data ? m_data.get() : &defaultBlueprint; }
    Blueprint* getMutable() const       { assert(m_data); return m_data.get(); }
    const Blueprint& operator*() const  { return *get(); }
    const Blueprint* operator->() const { return get(); }

signals:
    void pointerChanged();
    void isValidChanged(bool isValidNow);
    void isNullChanged(bool isNullNow);

private:
    static const Blueprint defaultBlueprint;
private:
    std::shared_ptr<Blueprint> m_data;
private:
    void emitChangedSignals(Blueprint* oldPtr);
};


#endif // BLUEPRINTBASIC_H
