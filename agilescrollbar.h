#ifndef AGILESCROLLBAR_H
#define AGILESCROLLBAR_H

#include <QScrollBar>


/*!
 * \brief Scroll bar that can extend range automatically to fit any desired value
 */
class AgileScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    explicit AgileScrollBar(QWidget* parentArg = 0);

    int goalMinimum() const;
    void setGoalMinimum(int newMinimum);

    int goalMaximum() const;
    void setGoalMaximum(int newMaximum);

    int goalRangeMiddle() const;

public slots:
    void forceValue(int newValue);
    void setValue(int newValue);
    void setValue(int newValue, bool force);

private:
    int m_goalMinimum = 0;
    int m_goalMaximum = 0;

private:
    using QScrollBar::setMinimum;
    using QScrollBar::setMaximum;
    using QScrollBar::setRange;

private slots:
    void updateRange(int curValue);
};

#endif // AGILESCROLLBAR_H
