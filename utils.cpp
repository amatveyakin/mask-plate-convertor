#include "utils.h"


QString stringWithInteger(int value, QString s1, QString s2, QString s3)
{
    if ((value / 10) % 10 == 1 || value % 10 == 0 || value % 10 >= 4)
        return s3.arg(value);
    if (value % 10 >= 2)
        return s2.arg(value);
    return s1.arg(value);
}
