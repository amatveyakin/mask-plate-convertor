#include "agilescrollbar.h"


AgileScrollBar::AgileScrollBar(QWidget* parentArg) :
    QScrollBar(parentArg)
{
    setGoalMinimum(minimum());
    setGoalMaximum(maximum());
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(updateRange(int)));
}

int AgileScrollBar::goalMinimum() const
{
    return m_goalMinimum;
}

void AgileScrollBar::setGoalMinimum(int newMinimum)
{
    m_goalMinimum = newMinimum;
    updateRange(value());
}

int AgileScrollBar::goalMaximum() const
{
    return m_goalMaximum;
}

void AgileScrollBar::setGoalMaximum(int newMaximum)
{
    m_goalMaximum = newMaximum;
    updateRange(value());
}

int AgileScrollBar::goalRangeMiddle() const
{
    return (goalMinimum() + goalMaximum()) / 2;
}

void AgileScrollBar::forceValue(int newValue)
{
    updateRange(newValue);
    setValue(newValue);
}

void AgileScrollBar::setValue(int newValue)
{
    QScrollBar::setValue(newValue);
}

void AgileScrollBar::setValue(int newValue, bool force)
{
    if (force)
        forceValue(newValue);
    else
        setValue(newValue);
}

void AgileScrollBar::updateRange(int curValue)
{
    setMinimum(qMin(goalMinimum(), curValue));
    setMaximum(qMax(goalMaximum(), curValue));
}
