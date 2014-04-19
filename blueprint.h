#ifndef BLUEPRINT_H
#define BLUEPRINT_H

#include <vector>

#include <QPolygon>
#include <QString>

struct Element
{
    Element() : width(-1) {}

    QPolygon polygon;
    int width;
};

class Blueprint
{
public:
    Blueprint();
    ~Blueprint();

    const std::vector<Element> &elements() const    { return m_elements; }
    QRect boundingRect() const                      { return m_boundingRect; }

    void appendLine(QPoint from, QPoint to, int width);
    void finishElement();
    void postProcess();

private:
    std::vector<Element> m_elements;
    QRect m_boundingRect;
};

#endif // BLUEPRINT_H
