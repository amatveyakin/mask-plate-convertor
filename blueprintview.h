#ifndef BLUEPRINTVIEW_H
#define BLUEPRINTVIEW_H

#include <QAbstractScrollArea>

class AgileScrollBar;
class Blueprint;


class BlueprintView : public QAbstractScrollArea
{
    Q_OBJECT
    typedef QAbstractScrollArea ParentT;

public:
    explicit BlueprintView(QWidget* parentArg = nullptr);

    void setBlueprint(const Blueprint* blueprint);

protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void wheelEvent(QWheelEvent*) override;
    virtual void resizeEvent(QResizeEvent*) override;

private:
    const Blueprint* m_blueprint;
    QRect m_canvasRect;
    double m_scale;

private:
    AgileScrollBar* myHorizontalScrollBar() const;
    AgileScrollBar* myVerticalScrollBar() const;
    QTransform blueprintToScreenTransform() const;
    QPoint screenToBlueprint(QPoint pos) const;
    QPoint blueprintToScreen(QPoint pos) const;
    QRect blueprintRect() const;
    double builtInSizeCoeff() const;
    double sizeCoeff() const;
    void zoom(double factor, QPoint fixedScreenPoint);
    void updateCanvasRect();
    void updateScrollBarRanges();
    void updateViewportGeometry();
    void coLocatePoints(QPoint blueprintPoint, QPoint screenPoint);     ///< update geometry and co-locate specified point together
};

#endif // BLUEPRINTVIEW_H
