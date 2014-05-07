#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QLineF>


const double eps = 1e-8;

template<typename T>
T sqr(const T& x)
{
    return x * x;
}


namespace geometry
{
    double squaredLength(QLineF line);

    double distance(QPointF p1, QPointF p2);
    double distance(QLineF line, QPointF point);

    double dot(QPointF p1, QPointF p2);
};


inline double geometry::distance(QPointF p1, QPointF p2)
{
    return QLineF(p1, p2).length();
}

inline double geometry::squaredLength(QLineF line)
{
    return sqr(line.x1() - line.x2()) + sqr(line.y1() - line.y2());
}

inline double geometry::dot(QPointF p1, QPointF p2)
{
    return p1.x() * p2.x() + p1.y() * p2.y();
}

#endif // GEOMETRY_H
