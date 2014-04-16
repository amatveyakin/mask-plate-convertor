#ifndef BLUEPRINTVIEW_H
#define BLUEPRINTVIEW_H

#include <QAbstractScrollArea>

class QPrinter;

class AgileScrollBar;
class Blueprint;


class BlueprintView : public QAbstractScrollArea
{
    Q_OBJECT
    typedef QAbstractScrollArea ParentT;

public:
    explicit BlueprintView(QWidget* parentArg = nullptr);

    void setBlueprint(const Blueprint* blueprint);

public slots:
    void setflipHorizontally(bool flip);
    void setflipVertically(bool flip);
    void renderBlueprint(QPaintDevice* target, const QRect& targetRect);
    void renderBlueprint(QPrinter* printer);

protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void wheelEvent(QWheelEvent*) override;
    virtual void resizeEvent(QResizeEvent*) override;

private:
    const Blueprint* m_blueprint;
    QRect m_canvasRect;
    double m_scale;
    bool m_flipHorizontally;
    bool m_flipVertically;

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
    void coLocatePoints(QPoint blueprintPoint, QPoint screenPoint);     ///< update geometry and co-locate specified point together

    void doRenderBlueprint(QPainter& painter, const QRect& targetRect, const QTransform& transform) const;
};

#endif // BLUEPRINTVIEW_H
