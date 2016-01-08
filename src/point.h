#ifndef POINT_H
#define POINT_H

#include <iostream>
using namespace std;

class Point
{
public:
    Point();
    Point(int x, int y);
    Point(const Point& p);
    void change(int x, int y);
    void copy(const Point& p);

    friend ostream& operator<<(ostream& os, Point& p)
    {
        return os << "(" << p.xval << "," << p.yval << ")";
    }

    int xval, yval;

};

#endif // POINT_H
