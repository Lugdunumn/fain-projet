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
    if (x >= 0 && x <= w && y >= 0 && y <= h && img->_buffer[x][y] == old &&  img->_buffer[x][y] != c)
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
    if (x < 0 || x > w || y < 0 || y > h || c == old)
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

void Drawing::lineScanline(Image *img, int x1, int x2, int y, Color c, Color old, int w, int h)
{
    int xL,xR;
    if( y < 0 || y > h )
        return;
    for( xL = x1; xL >= 0; --xL )
    { // scan left
        if( img->_buffer[xL][y] != old )
            break;
        I_plotColor(img, xL, y, c);
    }
    if( xL < x1 ) {
        lineScanline(img, xL, x1, y-1, c, old, w, h); // fill child
        lineScanline(img, xL, x1, y+1, c, old, w, h); // fill child
        ++x1;
    }
    for( xR = x2;  xR <= w; ++xR ) { // scan right
        if( img->_buffer[xR][y] !=  old )
            break;
        I_plotColor(img, xR, y, c);
    }
    if( xR > x2 ) {
        lineScanline(img, x2, xR, y-1, c, old, w, h); // fill child
        lineScanline(img, x2, xR, y+1, c, old, w, h); // fill child
        --x2;
    }
    for( xR = x1; xR <= x2 && xR <= w; ++xR ) {  // scan betweens
        if( img->_buffer[xR][y] == old )
            I_plotColor(img, xR, y, c);
        else
        {
            if( x1 < xR ) {
                // fill child
                lineScanline(img, x1, xR-1, y-1, c, old, w, h);
                // fill child
                lineScanline(img, x1, xR-1, y+1, c, old, w, h);
                x1 = xR;
            }
            // Note: This function still works if this step is removed.
            for( ; xR <= x2 && xR <= w; ++xR) { // skip over border
                if( img->_buffer[xR][y] == old ) {
                    x1 = xR--;
                    break;
                }
            }
        }
    }
}

void Drawing::floodFillScanline(Image *img, int x, int y, Color c, Color old, int w, int h)
{
    if (x < 0 || x > w || y < 0 || y > h || c == old)
        return;
    lineScanline(img, x, x, y, c, old, w, h);

//    vector<Point> stack;

//    int x1, y1;
//    bool spanLeft, spanRight;

//    stack.push_back(Point(x,y));

//    while(!stack.empty())
//    {
//        stack.pop_back();

//        y1 = y;
//        while(y1 >= 0 && img->_buffer[x][y1] == old)
//            y1 --;
//        y1++;
//        spanLeft = false;
//        spanRight = false;

//        while(y1 < h && img->_buffer[x][y1] == old)
//        {
//            img->_buffer[x][y1] = c;
//            if (!spanLeft && x > 0 && img->_buffer[x - 1][y1] == old)
//            {
//                if (img->_buffer[x - 1][y1] != c)
//                {
//                    stack.push_back(Point(x - 1, y1));
//                    spanLeft = true;
//                    x--;
//                }
//                else
//                    return;
//            }
//            else if(spanLeft && x > 0 && img->_buffer[x - 1][y1] != old)
//            {
//                spanLeft = false;
//            }
//            if (!spanRight && x < w && img->_buffer[x + 1][y1] == old)
//            {
//                if (img->_buffer[x+1][y1] != c)
//                {
//                    stack.push_back(Point(x + 1, y1));
//                    spanRight = true;
//                    x++;
//                }
//                else
//                    return;
//            }
//            else if(spanRight && x > 0 && img->_buffer[x + 1][y1] != old)
//            {
//                spanRight = false;
//            }
//            y1++;
//        }
//    }
}

void Drawing::createPolygon(Image *img, int x, int y, Color c, Polygon polygon)
{
    if(polygon_end != true)
    {
        Vertex v(x,y);
        polygon.vertices.push_back(v);
        if (polygon.vertices.size() != 1)
        {
            Vertex v1, v2;
            int i = polygon.vertices.size();
            v1 = polygon.vertices.at(i - 1);
            v2 = polygon.vertices.back();
            bresenham(img, v1.x, v1.y, v2.x, v2.y, c);
        }
    }
    else
    {
        polygon_end = false;
        polygon.count++;
    }
}
