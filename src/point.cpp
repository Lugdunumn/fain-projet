#include "point.h"

Point::Point(int x = 0, int y = 0)
{
    xval = x;
    yval = y;
}

Point::Point()
{
    this->xval = 0;
    this->yval = 0;
}

Point::Point(const Point& p)
{
    Point r(p.xval, p.yval);
}

void Point::change(int x, int y)
{
    this->xval = x;
    this->yval = y;
}

void Point::copy(const Point& p)
{
    this->xval = p.xval;
    this->yval = p.yval;
}

ostream& operator<<(ostream& os, Point& p)
{
    return os << "(" << p.xval << "," << p.yval + ")" << ends;
}
