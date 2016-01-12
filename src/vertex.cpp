#include "vertex.h"

Vertex::Vertex(int x, int y)
{
    this->x = x;
    this->y = y;
}

Vertex::Vertex()
{
    this->x = 0;
    this->y = 0;
}

Vertex::Vertex(const Vertex& p)
    : x (p.x)
    , y (p.y)
{
}

void Vertex::change(int x, int y)
{
    this->x = x;
    this->y = y;
}

void Vertex::copy(const Vertex& p)
{
    this->x = p.x;
    this->y = p.y;
}


