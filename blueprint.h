#ifndef BLUEPRINT_H
#define BLUEPRINT_H

#include <vector>

#include <QPolygon>
#include <QString>

struct Element
{
    QPolygon polygon;
    int width = -1;
};

class Blueprint
{
public:
    Blueprint();
    ~Blueprint();

    int nElements() const               { return m_elements.size(); }
    const Element& element(int index)   { return m_elements.at(index); }

    void appendLineTo(QPoint from, QPoint to, int width);
    void finishElement();
    void cleanUp();

    QString toAutocadCommandLineCommands() const;
    // TODO: Also convert to something that QPainter can draw

private:
    std::vector<Element> m_elements;
};

#endif // BLUEPRINT_H
