#include "drawing.h"

Drawing::Drawing()
{
}


bool operator==(Color c1, Color c2)
{
    if (c1._red == c2._red && c1._blue == c2._blue && c1._green == c2._green)
        return true;
    else
        return false;
}

bool operator!=(Color c1, Color c2)
{
    if (c1._red == c2._red && c1._blue == c2._blue && c1._green == c2._green)
        return false;
    else
        return true;
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

void Drawing::floodFillRec(Image *img, int x, int y, Color c, Color old, int w, int h)
{
    if (x >= 0 && x < w && y >= 0 && y < h && img->_buffer[x][y] == old &&  img->_buffer[x][y] != c)
    {
        I_plotColor(img, x, y, c);
        floodFillRec(img, x - 1, y, c, old, w, h);
        floodFillRec(img, x + 1, y, c, old, w, h);
        floodFillRec(img, x, y - 1, c, old, w, h);
        floodFillRec(img, x, y + 1, c, old, w, h);
    }
}

void Drawing::floodFillNonRec(Image *img, int x, int y, Color c, Color old, int w, int h)
{
    if (x < 0 || x >= w || y < 0 || y >= h || c == old)
        return;
    vector<Point> stack;

    stack.push_back(Point(x,y));

    int nx, ny;

    while(!stack.empty())
    {
        Point t = stack.back();

        stack.pop_back();

        I_plotColor(img, t.x, t.y, c);

//        if (nx < 0 || nx >= w || ny < 0 || ny >= h || c == img->_buffer[nx][ny])
//            continue;

        nx = t.x - 1;
        ny = t.y;

        if(nx >= 0 && nx < w && ny >= 0 && ny < h && img->_buffer[nx][ny] == old && c!= img->_buffer[nx][ny])
            stack.push_back(Point(nx, ny));
        nx = t.x + 1;
        ny = t.y;

        if(nx >= 0 && nx < w && ny >= 0 && ny < h && img->_buffer[nx][ny] == old && c!= img->_buffer[nx][ny])
            stack.push_back(Point(nx, ny));
        nx = t.x;
        ny = t.y - 1;

        if(nx >= 0 && nx < w && ny >= 0 && ny < h && img->_buffer[nx][ny] == old && c!= img->_buffer[nx][ny])
            stack.push_back(Point(nx, ny));
        nx = t.x;
        ny = t.y + 1;

        if(nx >= 0 && nx < w && ny >= 0 && ny < h && img->_buffer[nx][ny] == old && c!= img->_buffer[nx][ny])
            stack.push_back(Point(nx, ny));
    }
}

typedef struct { int x1, x2, y, dy; } LINESEGMENT;

#define MAXDEPTH 10000

#define PUSH(XL, XR, Y, DY, h) \
    if( sp < stack+MAXDEPTH && Y+(DY) >= 0 && Y+(DY) < h ) \
    { sp->x1 = XL; sp->x2 = XR; sp->y = Y; sp->dy = DY; ++sp; }

#define POP(XL, XR, Y, DY) \
    { --sp; XL = sp->x1; XR = sp->x2; Y = sp->y+(DY = sp->dy); }

void Drawing::floodFillScanline(Image *img, int x, int y, Color c, Color old, int w, int h)
{
    int left, x1, x2, dy;
    LINESEGMENT stack[MAXDEPTH], *sp = stack;

    if( old == c )
        return;

    if( x < 0 || x >= w || y < 0 || y >= h )
        return;

    PUSH(x, x, y, 1, h);        /* needed in some cases */
    PUSH(x, x, y+1, -1, h);    /* seed segment (popped 1st) */

    while( sp > stack )
    {
        POP(x1, x2, y, dy);

        for( x = x1; x >= 0 && img->_buffer[x][y] == old; --x )
            img->_buffer[x][y] = c;

        if( x >= x1 )
            goto SKIP;

        left = x+1;
        if( left < x1 )
            PUSH(y, left, x1-1, -dy, h);    /* leak on left? */

        x = x1+1;

        do
        {
            for( ; x < w && img->_buffer[x][y] == old; ++x )
                img->_buffer[x][y]= c;

            PUSH(left, x-1, y, dy, h);

            if( x > x2+1 )
                PUSH(x2+1, x-1, y, -dy, h);    /* leak on right? */

SKIP:        for( ++x; x <= x2 && img->_buffer[x][y] != old; ++x ) {;}

            left = x;
        } while( x<=x2 );
    }
}

void Drawing::floodFillPatternRec(Image *img, Image *pattern, int x, int y, Color old, int w, int h, int ox, int oy)
{
    int pX, pY;
    pX = x - ox + (pattern->_width /2);
    pY = y - oy + (pattern->_height /2);
//    while (pX < 0)
//        pX += pattern->_width /2;
//    while (pX >= pattern->_width)
//        pX -= pattern->_width /2;

//    while (pY < 0)
//        pY += pattern->_height /2;
//    while (pX >= pattern->_height)
//        pX -= pattern->_height /2;

    if (x >= 0 && x < w && y >= 0 && y < h && img->_buffer[x][y] == old && img->_buffer[x][y] != pattern->_buffer[pX][pY])
    {
        I_plotColor(img, x, y, pattern->_buffer[pX][pY]);
        floodFillPatternRec(img, pattern, x-1, y, old, w, h, ox, oy);
        floodFillPatternRec(img, pattern, x+1, y, old, w, h, ox, oy);
        floodFillPatternRec(img, pattern, x, y - 1, old, w, h, ox, oy);
        floodFillPatternRec(img, pattern, x, y + 1, old, w, h, ox, oy);
    }
}

void Drawing::drawPoly(Image *img, Polygon p, Color c)
{
    Vertex v1, v2;
    for (int i = 0; i < p.vertices.size(); i++)
    {
        if (i != p.vertices.size() - 1)
        {
            v1 = p.vertices.at(i);
            v2 = p.vertices.at(i+1);
        }
        else
        {
            v1 = p.vertices.at(i);
            v2 = p.vertices.at(0);
        }
        bresenham(img, v1.x, v1.y, v2.x, v2.y, c);
    }
}
