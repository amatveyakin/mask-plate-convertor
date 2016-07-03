#ifndef COLORUTILS_H
#define COLORUTILS_H

#include <QColor>


inline QColor mixColors(QColor x, QColor y, double coeff)
{
    return QColor::fromRgbF(x.redF()   * coeff + y.redF()   * (1. - coeff),
                            x.greenF() * coeff + y.greenF() * (1. - coeff),
                            x.blueF()  * coeff + y.blueF()  * (1. - coeff),
                            x.alphaF() * coeff + y.alphaF() * (1. - coeff));
}

#endif // COLORUTILS_H

