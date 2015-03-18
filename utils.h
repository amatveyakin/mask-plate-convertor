#ifndef UTILS_H
#define UTILS_H

#include <QString>


template<typename T>
QString prettyPrintNumber(T x)
{
    QString result = QString::number(x);
    result.replace("-", "−");
    return result;
}

#endif // UTILS_H
