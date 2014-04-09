#include <assert.h>

#include <QPainter>
#include <QScrollBar>
#include <QWheelEvent>

#include "blueprint.h"
#include "blueprintview.h"


static inline double rectToRectSizeCoeff(const QRect& rect1, const QRect& rect2)
{
    return qMin(double(rect2.width()) / rect1.width(), double(rect2.height()) / rect1.height());
}


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
    m_scale = 1.;
    updateViewportGeometry();
    coLocatePoints(blueprintRect().center(), m_canvasRect.center());
}

void BlueprintView::paintEvent(QPaintEvent*)
{
    QPainter painter(viewport());
//    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    painter.fillRect(viewport()->rect(), Qt::white);
    if (!m_blueprint)
        return;
    painter.setTransform(blueprintToScreenTransform());
    painter.setBrush(Qt::NoBrush);
    for (const Element& element : m_blueprint->elements()) {
        painter.setPen(QPen(Qt::black, element.width, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));  // TODO: PenCapStyle, PenJoinStyle - ?
        painter.drawPolyline(element.polygon);
    }
}

void BlueprintView::wheelEvent(QWheelEvent* ev)
{
    if (!m_blueprint)
        return;
    const double factorBase = 1.001;
    double factor = pow(factorBase, ev->delta());
    zoom(factor, ev->pos());
}

void BlueprintView::resizeEvent(QResizeEvent* /*ev*/)
{
    if (!m_blueprint)
        return;
    QPoint blueprintFixedPoint = screenToBlueprint(m_canvasRect.center());
    updateViewportGeometry();
    QPoint goalScreenPoint = m_canvasRect.center();
    coLocatePoints(blueprintFixedPoint, goalScreenPoint);
}

QTransform BlueprintView::blueprintToScreenTransform() const
{
    QTransform result;
    result.translate(m_canvasRect.center().x(), m_canvasRect.center().y());
    result.translate(horizontalScrollBar()->maximum() / 2 - horizontalScrollBar()->value(),
                     verticalScrollBar()->maximum() / 2 - verticalScrollBar()->value());
    result.scale(sizeCoeff(), sizeCoeff());
    result.translate(-blueprintRect().center().x(), -blueprintRect().center().y());
    return result;
}

QPoint BlueprintView::screenToBlueprint(QPoint pos) const
{
    return blueprintToScreenTransform().inverted().map(pos);
}

QPoint BlueprintView::blueprintToScreen(QPoint pos) const
{
    return blueprintToScreenTransform().map(pos);
}

QRect BlueprintView::blueprintRect() const
{
    return m_blueprint ? m_blueprint->boundingRect() : QRect();
}

double BlueprintView::builtInSizeCoeff() const
{
    return rectToRectSizeCoeff(blueprintRect(), m_canvasRect);
}

double BlueprintView::sizeCoeff() const
{
    return  builtInSizeCoeff() * m_scale;
}

void BlueprintView::zoom(double factor, QPoint fixedScreenPoint)
{
    QPoint fixedBlueprintPoint = screenToBlueprint(fixedScreenPoint);
    m_scale = qMax(m_scale * factor, 1.);
    updateViewportGeometry();
    coLocatePoints(fixedBlueprintPoint, fixedScreenPoint);
}

void BlueprintView::updateCanvasRect()
{
    // TODO: Fix negative rects
    const int canvasMargin = 8;
    m_canvasRect = viewport()->rect().adjusted(canvasMargin, canvasMargin, -canvasMargin, -canvasMargin);
}

void BlueprintView::updateScrollBarRanges()
{
    if (!m_blueprint) {
        horizontalScrollBar()->setMaximum(0);
        verticalScrollBar()->setMaximum(0);
        return;
    }
    horizontalScrollBar()->setPageStep(m_canvasRect.width());
    verticalScrollBar()->setPageStep(m_canvasRect.height());
    horizontalScrollBar()->setMaximum(qMax(0., blueprintRect().width() * sizeCoeff() - m_canvasRect.width()));
    verticalScrollBar()->setMaximum(qMax(0., blueprintRect().height() * sizeCoeff() - m_canvasRect.height()));
}

void BlueprintView::updateViewportGeometry()
{
    updateCanvasRect();
    updateScrollBarRanges();
    viewport()->update();
}

void BlueprintView::coLocatePoints(QPoint blueprintPoint, QPoint screenPoint)
{
    QPoint scroll = blueprintToScreen(blueprintPoint) - screenPoint;
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() + scroll.x());
    verticalScrollBar()->setValue(verticalScrollBar()->value() + scroll.y());
    viewport()->update();
}
