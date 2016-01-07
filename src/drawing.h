#ifndef DRAWING_H
#define DRAWING_H

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
};

#endif // DRAWING_H
