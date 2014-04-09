#ifndef BLUEPRINTVIEW_H
#define BLUEPRINTVIEW_H

#include <QAbstractScrollArea>

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
    double m_scale;

private:
    QRect blueprintBounds() const;
    QRect canvasRect() const;
    double builtInSizeCoeff() const;
    double sizeCoeff() const;
    void zoom(double factor, QPoint fixedPoint);
    void updateScrollBars();
    void updateAll();
};

#endif // BLUEPRINTVIEW_H
