#include "polygon.h"

Polygon::Polygon()
{
}

void Polygon::reduce(int h)
{
    int x,y, x_tmp, y_tmp;
    int xc = this->centroid.x;
    int yc = this->centroid.y;

    for (size_t i = 0; i < this->vertices.size(); i++)
    {
        x = this->vertices.at(i).x - xc;
        y = h - this->vertices.at(i).y - yc;
        x_tmp = x - x/5;
        y_tmp = y - y/5;
        this->vertices.at(i).x = x_tmp + xc;
        this->vertices.at(i).y = h - (y_tmp + yc);
    }

}

void Polygon::increase(int h)
{
    int x,y, x_tmp, y_tmp;
    int xc = this->centroid.x;
    int yc = this->centroid.y;

    for (size_t i = 0; i < this->vertices.size(); i++)
    {
        x = this->vertices.at(i).x - xc;
        y = h - this->vertices.at(i).y - yc;
        x_tmp = x + x/5;
        y_tmp = y + y/5;
        this->vertices.at(i).x = x_tmp + xc;
        this->vertices.at(i).y = h - (y_tmp + yc);
    }
}

void Polygon::quarter(int h)
{
    int x,y, x_tmp, y_tmp;
    int xc = this->centroid.x;
    int yc = this->centroid.y;

    for (size_t i = 0; i < this->vertices.size(); i++)
    {
        x = this->vertices.at(i).x - xc;
        y = h - this->vertices.at(i).y - yc;
        x_tmp = -y*1;
        y_tmp = x*1;
        this->vertices.at(i).x = x_tmp + xc;
        this->vertices.at(i).y = h - (y_tmp + yc);
    }
}

void Polygon::getOrientation()
{
    if(this->o == clockWise)
        std::cout << "Colockwise" << endl;
    else
        std::cout << "Anti-Clockwise" << endl;
}

bool Polygon::isConvex(int h)
{
    bool got_negetive = false;
    bool got_positive = false;
    int num_points = this->vertices.size();
    int B,C;
    int xA, yA, xB, yB, xC, yC;
    int cross_product;

    for (int A = 0; A < num_points; A++)
    {
        B = (A + 1) % num_points;
        C = (B + 1) % num_points;

        xA = this->vertices.at(A).x;
        yA = h - this->vertices.at(A).y;

        xB = this->vertices.at(B).x;
        yB = h - this->vertices.at(B).y;

        xC = this->vertices.at(C).x;
        yC = h - this->vertices.at(C).y;

        cross_product = (xA - xB)*(yC - yB) - (yA - yB)*(xC - xB);

        if (cross_product < 0)
            got_negetive = true;
        else if (cross_product > 0)
            got_positive = true;
        if (got_negetive && got_positive) return false;
    }
    return true;

}
