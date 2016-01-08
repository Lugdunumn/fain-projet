#ifndef DRAWING_H
#define DRAWING_H

#include <iostream>
#include <vector>

using namespace std;

struct Point
{
    Point(int x_, int y_)
    : x(x_)
    , y(y_)
    {}

    int x;
    int y;
};

extern "C" {
#include "Image.h"
#include "Ppm.h"
}

class Drawing
{
public:
    Drawing();

    bool bresenham_phase_1 = true;
    bool bresenham_phase_2 = false;
    int bresenham_x1 = 0;
    int bresenham_y1 = 0;
    void bresenham(Image *img, int x1, int y1, int x2, int y2, Color c);

    int radius = 80;
    void circle(Image *img, int x0, int y0, int radius, Color c);

    void floodFillRec(Image *img, int x, int y, Color c, int w, int h);

    void floodFillNonRec(Image *img, int x, int y, Color c, int w, int h);

};

#endif // DRAWING_H
