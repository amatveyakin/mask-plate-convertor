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
    explicit AgileScrollBar(QWidget *parent = 0);

    int goalMinimum() const;
    void setGoalMinimum(int newMinimum);

    int goalMaximum() const;
    void setGoalMaximum(int newMaximum);

    int goalRangeMiddle() const;

public slots:
    void forceValue(int newValue);

private:
    int m_goalMinimum;
    int m_goalMaximum;

private:
    using QScrollBar::setMinimum;
    using QScrollBar::setMaximum;
    using QScrollBar::setRange;

private slots:
    void updateRange(int curValue);
};

#endif // AGILESCROLLBAR_H
