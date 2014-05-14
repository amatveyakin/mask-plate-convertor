#ifndef BLUEPRINTVIEW_H
#define BLUEPRINTVIEW_H

#include <QAbstractScrollArea>

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

public slots:
    void setFlipHorizontally(bool flip);
    void setFlipVertically(bool flip);
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
    BlueprintPtr m_blueprint;
    QRect m_canvasRect;
    double m_scale;
    bool m_flipHorizontally;
    bool m_flipVertically;
    SegmentId m_hoveredSegment;
    SegmentId m_selectedSegment;

private:
    AgileScrollBar* myHorizontalScrollBar() const;
    AgileScrollBar* myVerticalScrollBar() const;

    QTransform flipTransform() const;
    QTransform blueprintToRectTransform(const QRect& targetRect) const;
    QTransform blueprintToScreenTransform() const;
    QPoint screenToBlueprint(QPoint point) const;
    QPoint blueprintToScreen(QPoint point) const;

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

    void renderSegment(QPainter& painter, SegmentId segmentId, QColor color) const;
    void doRenderBlueprint(QPainter& painter, const QRect& targetRect, const QTransform& transform) const;
};

#endif // BLUEPRINTVIEW_H
