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

QString stringWithInteger(int value, QString s1, QString s2, QString s3);

#endif // UTILS_H
