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
    QPainter painter(this);
//    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    painter.fillRect(rect(), Qt::white);
    if (!m_blueprint)
        return;
    const int margin = 4;
    QRect blueprintBounds = m_blueprint->boundingRect();
    QRect canvasRect = rect().adjusted(margin, margin, -margin, -margin);
    double sizeCoeff = qMin(double(canvasRect.width()) / blueprintBounds.width(), double(canvasRect.height()) / blueprintBounds.height());
    painter.translate(margin, margin);
    painter.scale(sizeCoeff, sizeCoeff);
    painter.translate(-blueprintBounds.left(), -blueprintBounds.top());
    painter.setBrush(Qt::NoBrush);
    for (const Element& element : m_blueprint->elements()) {
        painter.setPen(QPen(Qt::black, element.width));  // TODO: PenCapStyle, PenJoinStyle - ?
        painter.drawPolyline(element.polygon);
    }
}
