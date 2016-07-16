#include <assert.h>

#include <QPainter>
#include <QPrinter>
#include <QScrollBar>
#include <QWheelEvent>

#include "agilescrollbar.h"
#include "blueprint.h"
#include "blueprintview.h"
#include "geometry.h"


static inline double rectToRectSizeCoeff(const QRect& rect1, const QRect& rect2)
{
    return qMin(double(rect2.width()) / rect1.width(), double(rect2.height()) / rect1.height());
}


BlueprintView::BlueprintView(QWidget* parentArg)
    : ParentT(parentArg)
{
    setMouseTracking(true);

    setHorizontalScrollBar(new AgileScrollBar(this));
    setVerticalScrollBar(new AgileScrollBar(this));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void BlueprintView::setBlueprint(BlueprintPtr blueprint)
{
    m_blueprint = blueprint;
    m_scale = 1.;
    m_hoveredSegment = SegmentId::invalid();
    m_selectedSegment = SegmentId::invalid();
    updateViewportGeometry();
    coLocatePoints(blueprintRect().center(), m_canvasRect.center(), true);
}

void BlueprintView::setHighlight(std::vector<SegmentId> segments)
{
    m_externallyHighlightedSegments = std::move(segments);
    updateViewport();
}

void BlueprintView::setFlipHorizontally(bool flip)
{
    m_flipHorizontally = flip;
    updateViewport();
}

void BlueprintView::setFlipVertically(bool flip)
{
    m_flipVertically = flip;
    updateViewport();
}

void BlueprintView::setShowTransitions(bool showTrans)
{
    m_showTransitions = showTrans;
    updateViewport();
}

void BlueprintView::renderBlueprint(QPaintDevice* target, const QRect& targetRect)
{
    QPainter painter(target);
    doRenderBlueprint(painter, targetRect, blueprintToRectTransform(targetRect), false);
}

void BlueprintView::renderBlueprint(QPrinter* printer)
{
    QRect targetRect = printer->pageRect();
    targetRect.moveTopLeft({0, 0});
    renderBlueprint(printer, targetRect);
}

void BlueprintView::paintEvent(QPaintEvent*)
{
    QPainter painter(viewport());
    doRenderBlueprint(painter, viewport()->rect(), blueprintToScreenTransform(), true);
}

void BlueprintView::mousePressEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton) {
        updateHoveredSegment();
        if (m_selectedSegment != m_hoveredSegment) {
            m_selectedSegment = m_hoveredSegment;
            emit selectedSegmentChanged(m_selectedSegment);
            updateViewport();
        }
    } else if (ev->button() == Qt::RightButton) {
        m_isPanning = true;
        m_panPivot = screenToBlueprint(ev->pos());
        setCursor(Qt::ClosedHandCursor);
    }
}

void BlueprintView::mouseReleaseEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::RightButton) {
        m_isPanning = false;
        unsetCursor();
    }
}

void BlueprintView::mouseMoveEvent(QMouseEvent* ev)
{
    if (m_isPanning) {
        coLocatePoints(m_panPivot, ev->pos(), false);
    }
    updateViewport();
}

void BlueprintView::wheelEvent(QWheelEvent* ev)
{
    if (m_blueprint.isNull())
        return;
    const double factorBase = 1.001;
    double factor = pow(factorBase, ev->delta());
    zoom(factor, ev->pos());
}

void BlueprintView::resizeEvent(QResizeEvent* /*ev*/)
{
    QPoint blueprintFixedPoint = screenToBlueprint(m_canvasRect.center());
    updateViewportGeometry();
    QPoint goalScreenPoint = m_canvasRect.center();
    coLocatePoints(blueprintFixedPoint, goalScreenPoint, true);
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

BlueprintView::SegmentAvatar BlueprintView::segmentAvatar(SegmentId segmentId, QColor color) const
{
    assert(m_blueprint->isSegmentValid(segmentId));
    const Element& element = m_blueprint->elements()[segmentId.element];
    // TODO: Use Qt::SquareCap for segment and QPainterPathStroker to clip current element
    return {QLine(element.polygon[segmentId.segment], element.polygon[segmentId.segment + 1]),
            QPen(color, element.width, Qt::SolidLine, Qt::FlatCap)};
}

QRect BlueprintView::blueprintRect() const
{
    return m_blueprint->boundingRect();
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
    coLocatePoints(fixedBlueprintPoint, fixedScreenPoint, true);
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
    myHorizontalScrollBar()->setPageStep(m_canvasRect.width());
    myVerticalScrollBar()->setPageStep(m_canvasRect.height());
    myHorizontalScrollBar()->setGoalMaximum(qMax(0., blueprintRect().width() * sizeCoeff() - m_canvasRect.width()));
    myVerticalScrollBar()->setGoalMaximum(qMax(0., blueprintRect().height() * sizeCoeff() - m_canvasRect.height()));
}

void BlueprintView::updateViewportGeometry()
{
    updateCanvasRect();
    updateScrollBarRanges();
    updateViewport();
}

void BlueprintView::coLocatePoints(QPoint blueprintPoint, QPoint screenPoint, bool force)
{
    QPoint scrollValue = blueprintToScreen(blueprintPoint) - screenPoint;
    myHorizontalScrollBar()->setValue(myHorizontalScrollBar()->value() + scrollValue.x(), force);
    myVerticalScrollBar()->setValue(myVerticalScrollBar()->value() + scrollValue.y(), force);
    updateViewport();
}

void BlueprintView::updateHoveredSegment()
{
    const double reselectionPenaltyScreen = 2.;
    const double maxDistanceScreen = 4.;
    QPointF cursorPos = screenToBlueprint(viewport()->mapFromGlobal(QCursor::pos()));
    const double reselectionPenalty = reselectionPenaltyScreen / sizeCoeff();
    double bestDistance = maxDistanceScreen / sizeCoeff();
    SegmentId bestSegment = SegmentId::invalid();
    for (int i = 0; i < int(m_blueprint->elements().size()); ++i) {
        const Element& element = m_blueprint->elements()[i];
        for (int j = 0; j < element.polygon.size() - 1; ++j) {
            SegmentId segmentId = {i, j};
            double d = geometry::distance(QLineF(element.polygon[j], element.polygon[j + 1]), element.width, cursorPos);
            if (segmentId == m_selectedSegment)
                d += reselectionPenalty;
            if (d < bestDistance) {
                bestDistance = d;
                bestSegment = segmentId;
            }
        }
    }
    if (m_hoveredSegment != bestSegment) {
        m_hoveredSegment = bestSegment;
        emit hoveredSegmentChanged(m_hoveredSegment);
    }
}

void BlueprintView::updateViewport()
{
    updateHoveredSegment();
    viewport()->update();
}

void BlueprintView::renderSegment(QPainter& painter, const SegmentAvatar& avatar) const
{
    painter.setPen(avatar.pen);
    painter.drawLine(avatar.coords);
}

void BlueprintView::renderSegment(QPainter& painter, SegmentId segmentId, QColor color) const
{
    renderSegment(painter, segmentAvatar(segmentId, color));
}

void BlueprintView::renderExternallyHighlightedSegments(QPainter& painter) const
{
    if (m_externallyHighlightedSegments.empty())
        return;

    int minX = std::numeric_limits<int>::max();
    int minY = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int maxY = std::numeric_limits<int>::min();
    for (SegmentId segmentId : m_externallyHighlightedSegments) {
        SegmentAvatar avatar = segmentAvatar(segmentId, QColor::fromRgbF(0.6, 0.6, 1., 0.8));
        avatar.pen.setWidthF(avatar.pen.widthF() + 60);
        avatar.pen.setCapStyle(Qt::RoundCap);
        renderSegment(painter, avatar);
        for (QPoint p : {avatar.coords.p1(), avatar.coords.p2()}) {
            minX = std::min(minX, p.x());
            minY = std::min(minY, p.y());
            maxX = std::max(maxX, p.x());
            maxY = std::max(maxY, p.y());
        }
    }
    const int infinity = 1000000000;
    const int border = 150;
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor::fromRgbF(0.6, 0.8, 1., 0.15));
    painter.drawRect(QRect(QPoint(minX - border, -infinity),
                           QPoint(maxX + border,  infinity)));
    painter.drawRect(QRect(QPoint(-infinity, minY - border),
                           QPoint( infinity, maxY + border)));
}

void BlueprintView::doRenderBlueprint(QPainter& painter, const QRect& targetRect, const QTransform& transform, bool showDecorations) const
{
    painter.fillRect(targetRect, Qt::white);  // TODO: Should we do it here? It will spoil SVG for example
    painter.setTransform(transform);
    painter.setBrush(Qt::NoBrush);

    if (showDecorations) {
        renderExternallyHighlightedSegments(painter);

        if (m_showTransitions) {
            painter.setRenderHint(QPainter::Antialiasing, true);
            QLinearGradient brush;
            brush.setColorAt(0., QColor::fromRgbF(0.6, 0.6, 0.,   1.));
            brush.setColorAt(1., QColor::fromRgbF(0.7, 0.7, 0.15, 0.5));
            for (int i = 0; i < (int)m_blueprint->elements().size() - 1; ++i) {
                QPoint start = m_blueprint->elements()[i].polygon.back();
                QPoint stop = m_blueprint->elements()[i + 1].polygon.front();
                brush.setStart(start);
                brush.setFinalStop(stop);
                painter.setPen(QPen(brush, 15, Qt::DotLine, Qt::FlatCap, Qt::BevelJoin));
                painter.drawLine(start, stop);
            }
        }
    }

    painter.setRenderHint(QPainter::Antialiasing, false);
    for (const Element& element : m_blueprint->elements()) {
        painter.setPen(QPen(Qt::black, element.width, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));
        painter.drawPolyline(element.polygon);
    }

    if (showDecorations) {
        if (m_selectedSegment == m_hoveredSegment) {
            if (m_blueprint->isSegmentValid(m_selectedSegment))
                renderSegment(painter, m_selectedSegment, QColor::fromRgbF(1., 0.2, 0.2));
        }
        else {
            if (m_blueprint->isSegmentValid(m_selectedSegment))
                renderSegment(painter, m_selectedSegment, QColor::fromRgbF(1., 0., 0.));
            if (m_blueprint->isSegmentValid(m_hoveredSegment))
                renderSegment(painter, m_hoveredSegment, QColor::fromRgbF(0.7, 0.7, 0.7));
        }
    }
}
