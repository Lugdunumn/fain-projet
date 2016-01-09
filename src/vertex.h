#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
using namespace std;

class Vertex
{
public:
    Vertex();
    Vertex(int x, int y);
    Vertex(const Vertex&);
    void change(int x, int y);
    void copy(const Vertex& p);

    friend ostream& operator<<(ostream& os, Vertex& v)
    {
        return os << "(" << v.x << "," << v.y << ")";
    }

    int x, y;

};

#endif
