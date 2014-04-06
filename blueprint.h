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

    void appendLine(QPoint from, QPoint to, int width);
    void finishElement();
    void cleanUp();

    QString toAutocadCommandLineCommands() const;

private:
    std::vector<Element> m_elements;
};

#endif // BLUEPRINT_H
