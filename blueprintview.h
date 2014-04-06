#ifndef BLUEPRINTVIEW_H
#define BLUEPRINTVIEW_H

#include <QWidget>

class Blueprint;


class BlueprintView : public QWidget
{
    Q_OBJECT

public:
    explicit BlueprintView(QWidget* parentArg = nullptr);

    void setBlueprint(const Blueprint* blueprint);

protected:
    virtual void paintEvent(QPaintEvent *) override;

private:
    const Blueprint* m_blueprint = nullptr;
};

#endif // BLUEPRINTVIEW_H
