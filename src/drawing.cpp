#include "drawing.h"

Drawing::Drawing()
{
}

void Drawing::bresenham(Image *img, int x1, int y1, int x2, int y2, Color c)
{
    int dx, dy, i, e;
    int incx, incy, inc1, inc2;
    int x,y;

    dx = x2 - x1;
    dy = y2 - y1;

    if(dx < 0) dx = -dx;
    if(dy < 0) dy = -dy;
    incx = 1;
    if(x2 < x1) incx = -1;
    incy = 1;
    if(y2 < y1) incy = -1;
    x=x1;
    y=y1;

    if(dx > dy)
      {
        I_plotColor(img, x, y, c);
        e = 2*dy - dx;
        inc1 = 2*( dy -dx);
        inc2 = 2*dy;
        for(i = 0; i < dx; i++)
        {
           if(e >= 0)
           {
              y += incy;
              e += inc1;
           }
           else e += inc2;
           x += incx;
           I_plotColor(img, x, y, c);
        }
     }
     else
     {
        I_plotColor(img, x, y, c);
        e = 2*dx - dy;
        inc1 = 2*( dx - dy);
        inc2 = 2*dx;
        for(i = 0; i < dy; i++)
        {
          if(e >= 0)
          {
             x += incx;
             e += inc1;
          }
          else e += inc2;
          y += incy;
          I_plotColor(img, x, y, c);
      }
    }
}


void Drawing::circle(Image *img, int x0, int y0, int radius, Color c)
{
    int x = radius;
    int y = 0;
    int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0

    while( y <= x )
    {
      I_plotColor(img, x + x0, y + y0, c); // Octant 1
      I_plotColor(img, y + x0, x + y0, c); // Octant 2
      I_plotColor(img, -x + x0, y + y0, c); // Octant 4
      I_plotColor(img, -y + x0, x + y0, c); // Octant 3
      I_plotColor(img, -x + x0, -y + y0, c); // Octant 5
      I_plotColor(img, -y + x0, -x + y0, c); // Octant 6
      I_plotColor(img, x + x0, -y + y0, c); // Octant 8
      I_plotColor(img, y + x0, -x + y0, c); // Octant 7
      y++;
      if (decisionOver2<=0)
      {
        decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
      }
      else
      {
        x--;
        decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
      }
    }
}
