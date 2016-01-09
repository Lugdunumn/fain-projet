#ifndef POLYGON_H
#define POLYGON_H

#include <iostream>
#include <vector>

#include "vertex.h"

enum Orientation{ clockWise, antiClockWise};

enum Convexity{ convex, concave };

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

    void reduce();

    void increase();

    void quarter();
};

#endif // POLYGON_H
