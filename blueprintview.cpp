// TODO: May be, use placeholder empty blueprint when there is not blueprint (here and in main window)?

#include <assert.h>

#include <QPainter>
#include <QPrinter>
#include <QScrollBar>
#include <QWheelEvent>

#include "agilescrollbar.h"
#include "blueprint.h"
#include "blueprintview.h"


static inline double rectToRectSizeCoeff(const QRect& rect1, const QRect& rect2)
{
    return qMin(double(rect2.width()) / rect1.width(), double(rect2.height()) / rect1.height());
}


BlueprintView::BlueprintView(QWidget* parentArg)
    : ParentT(parentArg)
    , m_blueprint(nullptr)
    , m_scale(1.)
    , m_flipHorizontally(false)
    , m_flipVertically(false)
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

void BlueprintView::setFlipHorizontally(bool flip)
{
    m_flipHorizontally = flip;
    viewport()->update();
}

void BlueprintView::setFlipVertically(bool flip)
{
    m_flipVertically = flip;
    viewport()->update();
}

void BlueprintView::renderBlueprint(QPaintDevice* target, const QRect& targetRect)
{
    QPainter painter(target);
    doRenderBlueprint(painter, targetRect, blueprintToRectTransform(targetRect));
}

void BlueprintView::renderBlueprint(QPrinter* printer)
{
    QRect targetRect = printer->pageRect();
    targetRect.moveTopLeft(QPoint(0, 0));
    renderBlueprint(printer, targetRect);
}

void BlueprintView::paintEvent(QPaintEvent*)
{
    QPainter painter(viewport());
    doRenderBlueprint(painter, viewport()->rect(), blueprintToScreenTransform());
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
    return static_cast<AgileScrollBar*>(horizontalScrollBar());
}

AgileScrollBar* BlueprintView::myVerticalScrollBar() const
{
    return static_cast<AgileScrollBar*>(verticalScrollBar());
}

QTransform BlueprintView::flipTransform() const
{
    QTransform result;
    if (m_flipHorizontally)
        result.scale(-1., 1.);
    if (!m_flipVertically)   // we use other coordinate system
        result.scale(1., -1.);
    return result;
}

QTransform BlueprintView::blueprintToRectTransform(const QRect& targetRect) const
{
    QTransform result;
    result.translate(targetRect.center().x(), targetRect.center().y());
    double transformSizeCoeff = rectToRectSizeCoeff(blueprintRect(), targetRect);
    result.scale(transformSizeCoeff, transformSizeCoeff);
    result = flipTransform() * result;
    result.translate(-blueprintRect().center().x(), -blueprintRect().center().y());
    return result;
}

QTransform BlueprintView::blueprintToScreenTransform() const
{
    QTransform result;
    result.translate(m_canvasRect.center().x(), m_canvasRect.center().y());
    result.translate(myHorizontalScrollBar()->goalRangeMiddle() - myHorizontalScrollBar()->value(),
                     myVerticalScrollBar()->goalRangeMiddle() - myVerticalScrollBar()->value());
    result.scale(sizeCoeff(), sizeCoeff());
    result = flipTransform() * result;
    result.translate(-blueprintRect().center().x(), -blueprintRect().center().y());
    return result;
}

QPoint BlueprintView::screenToBlueprint(QPoint point) const
{
    return blueprintToScreenTransform().inverted().map(point);
}

QPoint BlueprintView::blueprintToScreen(QPoint point) const
{
    return blueprintToScreenTransform().map(point);
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
    QPoint scrollValue = blueprintToScreen(blueprintPoint) - screenPoint;
    myHorizontalScrollBar()->forceValue(myHorizontalScrollBar()->value() + scrollValue.x());
    myVerticalScrollBar()->forceValue(myVerticalScrollBar()->value() + scrollValue.y());
    viewport()->update();
}

void BlueprintView::doRenderBlueprint(QPainter& painter, const QRect& targetRect, const QTransform& transform) const
{
    painter.fillRect(targetRect, Qt::white);  // TODO: Should we do it here? It will spoil SVG for example
    if (!m_blueprint)
        return;
    painter.setTransform(transform);
    painter.setBrush(Qt::NoBrush);
    for (const Element& element : m_blueprint->elements()) {
        painter.setPen(QPen(Qt::black, element.width, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));  // TODO: PenCapStyle, PenJoinStyle - ?
        painter.drawPolyline(element.polygon);
    }
}
