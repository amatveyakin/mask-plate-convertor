#include <QPainter>
#include <QScrollBar>
#include <QWheelEvent>

#include "blueprint.h"
#include "blueprintview.h"


static const int canvasMargin = 8;


BlueprintView::BlueprintView(QWidget *parentArg)
    : ParentT(parentArg)
    , m_blueprint(nullptr)
    , m_scale(1.)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void BlueprintView::setBlueprint(const Blueprint* blueprint)
{
    m_blueprint = blueprint;
    updateScrollBars();
}

void BlueprintView::paintEvent(QPaintEvent*)
{
    QPainter painter(viewport());
    QRect viewportRect = viewport()->rect();
//    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    painter.fillRect(viewportRect, Qt::white);
    if (!m_blueprint)
        return;
    double sizeCoeff = builtInSizeCoeff();
    sizeCoeff *= m_scale;
    painter.translate(canvasMargin, canvasMargin);
    painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
    painter.scale(sizeCoeff, sizeCoeff);
    painter.translate(-blueprintBounds().left(), -blueprintBounds().top());
    painter.setBrush(Qt::NoBrush);
    for (const Element& element : m_blueprint->elements()) {
        painter.setPen(QPen(Qt::black, element.width, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));  // TODO: PenCapStyle, PenJoinStyle - ?
        painter.drawPolyline(element.polygon);
    }
}

void BlueprintView::wheelEvent(QWheelEvent* ev)
{
    const double factorBase = 1.001;
    double factor = pow(factorBase, ev->delta());
    zoom(factor, ev->pos());
}

void BlueprintView::resizeEvent(QResizeEvent*)
{
    updateScrollBars();
}

QRect BlueprintView::blueprintBounds() const
{
    return m_blueprint->boundingRect();
}

QRect BlueprintView::canvasRect() const
{
    QRect viewportRect = viewport()->rect();
    return viewportRect.adjusted(canvasMargin, canvasMargin, -canvasMargin, -canvasMargin);
}

double BlueprintView::builtInSizeCoeff() const
{
    return qMin(double(canvasRect().width()) / blueprintBounds().width(), double(canvasRect().height()) / blueprintBounds().height());
}

void BlueprintView::zoom(double factor, QPoint /*fixedPoint*/)
{
    // TODO: preserve fixed point
    m_scale = qMax(m_scale * factor, 1.);
    updateScrollBars();
}

void BlueprintView::updateScrollBars()
{
    if (!m_blueprint) {
        horizontalScrollBar()->setMaximum(0);
        verticalScrollBar()->setMaximum(0);
        return;
    }
    double totalScale = builtInSizeCoeff() * m_scale;
    horizontalScrollBar()->setMaximum(qMax(0., blueprintBounds().width() * totalScale - canvasRect().width()));
    verticalScrollBar()->setMaximum(qMax(0., blueprintBounds().height() * totalScale - canvasRect().height()));
    viewport()->update();
}
