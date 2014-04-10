#include <assert.h>

#include <QPainter>
#include <QScrollBar>
#include <QWheelEvent>

#include "agilescrollbar.h"
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
    setHorizontalScrollBar(new AgileScrollBar(this));
    setVerticalScrollBar(new AgileScrollBar(this));
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

AgileScrollBar* BlueprintView::myHorizontalScrollBar() const
{
    return static_cast<AgileScrollBar *>(horizontalScrollBar());
}

AgileScrollBar* BlueprintView::myVerticalScrollBar() const
{
    return static_cast<AgileScrollBar *>(verticalScrollBar());
}

QTransform BlueprintView::blueprintToScreenTransform() const
{
    QTransform result;
    result.translate(m_canvasRect.center().x(), m_canvasRect.center().y());
    result.translate(myHorizontalScrollBar()->goalRangeMiddle() - myHorizontalScrollBar()->value(),
                     myVerticalScrollBar()->goalRangeMiddle() - myVerticalScrollBar()->value());
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
    const int canvasMargin = 8;
    const int minSize = 4;
    QRect viewportRect = viewport()->rect();
    m_canvasRect = viewportRect.adjusted(canvasMargin, canvasMargin, -canvasMargin, -canvasMargin);
    if (m_canvasRect.width() < minSize) {
        m_canvasRect.setLeft(viewportRect.center().x() - minSize / 2);
        m_canvasRect.setWidth(minSize);
    }
    if (m_canvasRect.height() < minSize) {
        m_canvasRect.setTop(viewportRect.center().y() - minSize / 2);
        m_canvasRect.setHeight(minSize);
    }
}

void BlueprintView::updateScrollBarRanges()
{
    myHorizontalScrollBar()->setGoalMinimum(0);
    myVerticalScrollBar()->setGoalMinimum(0);
    if (!m_blueprint) {
        myHorizontalScrollBar()->setGoalMaximum(0);
        myVerticalScrollBar()->setGoalMaximum(0);
        return;
    }
    myHorizontalScrollBar()->setPageStep(m_canvasRect.width());
    myVerticalScrollBar()->setPageStep(m_canvasRect.height());
    myHorizontalScrollBar()->setGoalMaximum(qMax(0., blueprintRect().width() * sizeCoeff() - m_canvasRect.width()));
    myVerticalScrollBar()->setGoalMaximum(qMax(0., blueprintRect().height() * sizeCoeff() - m_canvasRect.height()));
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
    myHorizontalScrollBar()->forceValue(myHorizontalScrollBar()->value() + scroll.x());
    myVerticalScrollBar()->forceValue(myVerticalScrollBar()->value() + scroll.y());
    viewport()->update();
}
