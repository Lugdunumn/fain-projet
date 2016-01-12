#ifndef POLYGON_H
#define POLYGON_H

#include <iostream>
#include <vector>
#include "math.h"

#include "vertex.h"

enum Orientation{ clockWise, antiClockWise};

enum Convexity{ concave, convex };

class Polygon
{
public:
    Polygon();

    int count = 0;

    bool isCreated = false;

    vector<Vertex> vertices;

    Vertex centroid;

    Orientation o;

    Convexity c;

    void reduce(int h);

    void increase(int h);

    void quarter(int h);

    void getOrientation();

    bool isConvex(int h);
};

#endif // POLYGON_H
