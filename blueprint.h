#ifndef BLUEPRINT_H
#define BLUEPRINT_H

#include <assert.h>

#include <memory>
#include <vector>

#include <QPolygon>
#include <QObject>

class BlueprintPtr;


struct Element
{
    Element() : width(-1) {}

    QPolygon polygon;
    int width;
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

    void appendLine(QPoint from, QPoint to, int width);
    void finishElement();
    void preProcess();
    void postProcess();

private:
    std::vector<Element> m_elements;
    QRect m_boundingRect;
};

/*!
 * \brief Blueprint shared pointer that can be always dereferenced
 */
class BlueprintPtr : public QObject
{
    Q_OBJECT

public:
    BlueprintPtr() {}
    explicit BlueprintPtr(Blueprint* ptr) : m_data(ptr) {}
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

#endif // BLUEPRINT_H
