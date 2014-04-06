#include <assert.h>

#include "blueprint.h"


Blueprint::Blueprint()
{
    m_elements.emplace_back();
}

Blueprint::~Blueprint()
{
}

void Blueprint::appendLine(QPoint from, QPoint to, int width)
{
    assert(!m_elements.empty());
    Element& curElement = m_elements.back();
    if (curElement.polygon.empty()) {
        curElement.polygon << from << to;
        curElement.width = width;
    }
    else {
        assert(curElement.polygon.back() == from);
        assert(curElement.width == width);
        curElement.polygon << to;
    }
}

void Blueprint::finishElement()
{
    assert(!m_elements.empty());
    if (!m_elements.back().polygon.empty())
        m_elements.emplace_back();
}

static QRect elementBoundingRect(const Element& element)
{
    int bonus = (element.width + 1) / 2;
    return element.polygon.boundingRect().adjusted(-bonus, -bonus, bonus, bonus);
}

void Blueprint::postProcess()
{
    assert(!m_elements.empty());
    if (m_elements.back().polygon.empty())
        m_elements.pop_back();
    m_boundingRect = elementBoundingRect(m_elements.front());
    for (const Element& element : m_elements)
        m_boundingRect |= elementBoundingRect(element);
}

QString Blueprint::toAutocadCommandLineCommands() const
{
    const double sizeCoeff = 0.001;
    QString result;
    for (const Element& curElement : m_elements) {
        for (int i = 0; i < curElement.polygon.size(); ++i) {
            QString coords = QString("%1,%2").arg(QString::number(curElement.polygon[i].x() * sizeCoeff, 'f'), QString::number(curElement.polygon[i].y() * sizeCoeff, 'f'));
            if (i == 0)
                result.append(QString("_pline %1 width %2 %2\n").arg(coords, QString::number(curElement.width * sizeCoeff)));
            else
                result.append(QString("%1\n").arg(coords));
        }
        result.append("\n");
    }
    return result;
}
