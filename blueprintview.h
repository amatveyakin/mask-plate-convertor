#ifndef BLUEPRINTVIEW_H
#define BLUEPRINTVIEW_H

#include <QAbstractScrollArea>
#include <QPen>

#include "blueprintbasic.h"

class QPrinter;

class AgileScrollBar;


class BlueprintView : public QAbstractScrollArea
{
    Q_OBJECT
    typedef QAbstractScrollArea ParentT;

public:
    explicit BlueprintView(QWidget* parentArg = nullptr);

    void setBlueprint(BlueprintPtr blueprint);
    void setHighlight(std::vector<SegmentId> segments);

public slots:
    void setFlipHorizontally(bool flip);
    void setFlipVertically(bool flip);
    void setShowTransitions(bool showTrans);
    void renderBlueprint(QPaintDevice* target, const QRect& targetRect);
    void renderBlueprint(QPrinter* printer);

signals:
    void hoveredSegmentChanged(SegmentId segment);
    void selectedSegmentChanged(SegmentId segment);

protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
    virtual void wheelEvent(QWheelEvent*) override;
    virtual void resizeEvent(QResizeEvent*) override;

private:
    struct SegmentAvatar {
        QLine coords;
        QPen pen;
    };

    BlueprintPtr m_blueprint;
    QRect m_canvasRect;
    double m_scale = 1.;
    bool m_flipHorizontally = false;
    bool m_flipVertically = false;
    bool m_showTransitions = false;
    SegmentId m_hoveredSegment;
    SegmentId m_selectedSegment;
    std::vector<SegmentId> m_externallyHighlightedSegments;

private:
    AgileScrollBar* myHorizontalScrollBar() const;
    AgileScrollBar* myVerticalScrollBar() const;

    QTransform flipTransform() const;
    QTransform blueprintToRectTransform(const QRect& targetRect) const;
    QTransform blueprintToScreenTransform() const;
    QPoint screenToBlueprint(QPoint point) const;
    QPoint blueprintToScreen(QPoint point) const;

    SegmentAvatar segmentAvatar(SegmentId segmentId, QColor color) const;
    QRect blueprintRect() const;
    double builtInSizeCoeff() const;
    double sizeCoeff() const;

    void zoom(double factor, QPoint fixedScreenPoint);
    void updateCanvasRect();
    void updateScrollBarRanges();
    void updateViewportGeometry();
    void coLocatePoints(QPoint blueprintPoint, QPoint screenPoint);
    void updateHoveredSegment();
    void updateViewport();

    void renderSegment(QPainter& painter, const SegmentAvatar& avatar) const;
    void renderSegment(QPainter& painter, SegmentId segmentId, QColor color) const;
    void renderExternallyHighlightedSegments(QPainter& painter) const;
    void doRenderBlueprint(QPainter& painter, const QRect& targetRect, const QTransform& transform, bool showDecorations) const;
};

#endif // BLUEPRINTVIEW_H
