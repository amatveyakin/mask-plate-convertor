#include "geometry.h"


double geometry::distance(QLineF line, QPointF p)
{
    const QPointF v = line.p1();
    const QPointF w = line.p2();

    const double l2 = squaredLength(line);
    if (fabs(l2) < eps)
        return distance(p, v);


    const double t = dot(p - v, w - v) / l2;
    if (t < 0.)
        return distance(p, v);
    else if (t > 1.)
        return distance(p, w);

    const QPointF projection = v + t * (w - v);
    return distance(p, projection);
}
