#include <math.h>
#include "./cylinder.hpp"

Point::Point (float x, float y, float z)
{
    this->x = x; this->y = y; this->z = z;
}

Point::Point ()
{
    x = y = z = 0;
}

Cylinder::Cylinder (float xCenter, float yCenter, float rx, float ry, float h, float ddx, float ddy)
{
    this->xCenter = xCenter; this->yCenter = yCenter; this->rx = rx; this->ry = ry;
    this->h = h; this->ddx = ddx; this->ddy = ddy;
    recalc(36, 18, 12);
}

std::vector<std::vector<std::vector<Point> > >& Cylinder::getLinesArr ()
{
    return this->linesArr;
}

std::vector<std::vector<std::vector<Point> > >& Cylinder::getfEllArr ()
{
    return this->fEllArr;
}

std::vector<std::vector<std::vector<Point> > >& Cylinder::getsEllArr ()
{
    return this->sEllArr;
}

void Cylinder::recalc (int n, int k, int kr)
{
    //first ellipse
    fEllArr.clear();
    float rxStep = rx / kr, ryStep = ry / kr;
    float rxi = rxStep, ryi = ryStep;
    for (int i = 0; i < kr; i++)
    {
        std::vector<std::vector<Point> > sector;
        int curAngle = 0;
        for (int j = 0; j < kr; j++)
        {
            std::vector<Point> block;
            int maxAngle = 360 / kr * (j + 1);
            if (j == kr - 1)
                maxAngle = 360;
            for (int k = curAngle; k <= maxAngle; k++)
            {
                float angle = float(k * M_PI) / 180;
                float dx = rxi * cosf(angle), dy = ryi * sinf(angle);
                block.push_back(Point(dx + xCenter, 0, dy + yCenter));
            }
            curAngle = maxAngle;
            block.insert(block.begin(), Point(xCenter, 0, yCenter));
            sector.push_back(block);
        }
        fEllArr.push_back(sector);
        rxi += rxStep, ryi += ryStep;
    }
    
    //second ellipse
    sEllArr.clear();
    rxStep = rx / kr, ryStep = ry / kr;
    rxi = rxStep, ryi = ryStep;
    for (int i = kr - 1; i >= 0; i--)
    {
        std::vector<std::vector<Point> > sector;
        int curAngle = 360;
        for (int j = kr - 1; j >= 0; j--)
        {
            std::vector<Point> block;
            int minAngle = (360 / kr) * j;
            if (j == 0)
                minAngle = 0;
            for (int k = curAngle; k >= minAngle; k--)
            {
                float angle = float(k * M_PI) / 180;
                float dx = rxi * cosf(angle), dy = ryi * sinf(angle);
                block.push_back(Point(dx + xCenter + ddx, h, dy + yCenter + ddy));
            }
            curAngle = minAngle;
            block.insert(block.begin(), Point(xCenter + ddx, h, yCenter + ddy));
            sector.push_back(block);
        }
        sEllArr.push_back(sector);
        rxi += rxStep, ryi += ryStep;
    }

    //lines
    linesArr.clear();
    float step = float(2 * M_PI) / n;
    for (float angle = 0; angle <= float(2 * M_PI); angle += step)
    {
        std::vector<std::vector<Point> > sector;
        float a = (fabsf(angle - float(2 * M_PI)) < 0.00001f) ? 0.f : angle;
        float dx = rx * cosf(a), dy = ry * sinf(a);
        angle += step;
        a = (fabsf(angle - float(2 * M_PI)) < 0.00001f) ? 0.f : angle;
        float dx1 = rx * cosf(a), dy1 = ry * sinf(a);
        angle -= step;
        float hStep = float(h / k), ddxStep = float(ddx / k), ddyStep = float(ddy / k);
        unsigned int j = 0;
        for (float i = 0.f, x = xCenter, y = yCenter; j < k; i += hStep, x += ddxStep, y += ddyStep, j++)
        {
            std::vector<Point> block;
            Point p1, p2, p3, p4;
            p1 = Point(dx + x, i, dy + y);
            p2 = Point(dx + x + ddxStep, i + hStep, dy + y + ddyStep);
            p3 = Point(dx1 + x + ddxStep, i + hStep, dy1 + y + ddyStep);
            p4 = Point(dx1 + x, i, dy1 + y);
            block.push_back(p1);
            block.push_back(p2);
            block.push_back(p3);
            block.push_back(p4);
            sector.push_back(block);

        }
        linesArr.push_back(sector);
    }
}
