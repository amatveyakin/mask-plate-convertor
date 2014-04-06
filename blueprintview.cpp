#include <QPainter>

#include "blueprint.h"
#include "blueprintview.h"


BlueprintView::BlueprintView(QWidget *parentArg)
    : QWidget(parentArg)
    , m_blueprint(nullptr)
{
}

void BlueprintView::setBlueprint(const Blueprint* blueprint)
{
    m_blueprint = blueprint;
    update();
}

void BlueprintView::paintEvent(QPaintEvent*)
{
    const double sizeCoeff = 0.01;  // TODO: fix

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    if (!m_blueprint)
        return;
    painter.scale(sizeCoeff, sizeCoeff);
    painter.setBrush(Qt::NoBrush);
    for (const Element& element : m_blueprint->elements()) {
        painter.setPen(QPen(Qt::black, element.width));  // TODO: PenCapStyle, PenJoinStyle - ?
        painter.drawPolyline(element.polygon);
    }
}
