#include <math.h>
#include "./geometry.hpp"

Point::Point (double x, double y, double z)
{
    this->x = x; this->y = y; this->z = z;
}

Point::Point ()
{
    x = y = z = 0;
}

Cylinder::Cylinder (double xCenter, double yCenter, double rx, double ry, double h, double ddx, double ddy)
{
    this->xCenter = xCenter; this->yCenter = yCenter;
    this->rx = rx; this->ry = ry;
    this->h = h; this->ddx = ddx; this->ddy = ddy;
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

void Cylinder::recalc (int n, int k)
{
    fEllArr.clear();
    sEllArr.clear();
    double rxStep = rx / k, ryStep = ry / k;
    double rxi = rxStep, ryi = ryStep;
    double step = (double)360 / n;
    double curAngle = 0;
    for (int i = 0; i < n; i++)
    {
        std::vector<std::vector<Point>> fSector, sSector;
        rxi = rxStep, ryi = ryStep;
        for (int j = 0; j < k; j++)
        {
            std::vector<Point> fBlock, sBlock;
            double fAngle = double(curAngle * M_PI) / 180;
            double sAngle = (i == n - 1) ? 0.f : double((curAngle + step) * M_PI / 180);
            double dx1 = rxi * cos(fAngle), dy1 = ryi * sin(fAngle);
            double dx2 = rxi * cos(sAngle), dy2 = ryi * sin(sAngle);
            fBlock.push_back(Point(xCenter, 0, yCenter));
            fBlock.push_back(Point(xCenter + dx1, 0, yCenter + dy1));
            fBlock.push_back(Point(xCenter + dx2, 0, yCenter + dy2));
            sBlock.push_back(Point(xCenter + ddx, h, yCenter + ddy));
            sBlock.push_back(Point(xCenter + ddx + dx2, h, yCenter + ddy + dy2));
            sBlock.push_back(Point(xCenter + ddx + dx1, h, yCenter + ddy + dy1));
            //--------
//            fBlock.push_back(Point(xCenter, yCenter, 0));
//            fBlock.push_back(Point(xCenter + dx1, yCenter + dy1, 0));
//            fBlock.push_back(Point(xCenter + dx2, yCenter + dy2, 0));
//            sBlock.push_back(Point(xCenter + ddx, yCenter + ddy, h));
//            sBlock.push_back(Point(xCenter + ddx + dx2, yCenter + ddy + dy2, h));
//            sBlock.push_back(Point(xCenter + ddx + dx1, yCenter + ddy + dy1, h));
            fSector.push_back(fBlock);
            sSector.push_back(sBlock);
            rxi += rxStep, ryi += ryStep;
        }
        curAngle += step;
        fEllArr.push_back(fSector);
        sEllArr.push_back(sSector);
    }
    
    linesArr.clear();
    curAngle = 0.f;
    for (int i = 0; i < n; i++)
    {
        std::vector<std::vector<Point>> sector;
        double fAngle = double(curAngle * M_PI) / 180;
        double sAngle = (i == n - 1) ? 0.f : double((curAngle + step) * M_PI / 180);
        double dx1 = rx * cos(fAngle), dy1 = ry * sin(fAngle);
        double dx2 = rx * cos(sAngle), dy2 = ry * sin(sAngle);
        double hStep = (double)h / k, ddxStep = ddx / (double)k, ddyStep = ddy / (double)k;
        double x = xCenter, y = yCenter, curH = 0;
        for (int j = 0; j < k; j++)
        {
            std::vector<Point> block;
            block.push_back(Point(dx1 + x, curH, dy1 + y));
            block.push_back(Point(dx1 + x + ddxStep, curH + hStep, dy1 + y + ddyStep));
            block.push_back(Point(dx2 + x + ddxStep, curH + hStep, dy2 + y + ddyStep));
            block.push_back(Point(dx2 + x, curH, dy2 + y));
            //-----------
//            block.push_back(Point(dx1 + x, dy1 + y, curH));
//            block.push_back(Point(dx1 + x + ddxStep, dy1 + y + ddyStep, curH + hStep));
//            block.push_back(Point(dx2 + x + ddxStep, dy2 + y + ddyStep, curH + hStep));
//            block.push_back(Point(dx2 + x, dy2 + y, curH));
            curH += hStep;
            x += ddxStep;
            y += ddyStep;
            sector.push_back(block);
        }
        linesArr.push_back(sector);
        curAngle += step;
    }
}

std::vector<Point>& Segment::getPointsIn()
{
    return this->pointsIn;
}

std::vector<Point>& Segment::getPointsOut()
{
    return this->pointsOut;
}

Segment::Segment(double x1, double y1, double z1, double x2, double y2, double z2)
{
    this->x1 = x1; this->y1 = y1; this->z1 = z1;
    this->x2 = x2; this->y2 = y2; this->z2 = z2;
    pointsOut.push_back(Point(x1, y1, z1));
    pointsOut.push_back(Point(x2, y2, z2));
}

double Segment::getX1()
{
    return this->x1;
}

double Segment::getX2()
{
    return this->x2;
}

double Segment::getY1()
{
    return this->y1;
}

double Segment::getY2()
{
    return this->y2;
}

double Segment::getZ1()
{
    return this->z1;
}

double Segment::getZ2()
{
    return this->z2;
}

void Segment::setX1(double n)
{
    this->x1 = n;
    setDefault();
}

void Segment::setX2(double n)
{
    this->x2 = n;
    setDefault();
}

void Segment::setY1(double n)
{
    this->y1 = n;
    setDefault();
}

void Segment::setY2(double n)
{
    this->y2 = n;
    setDefault();
}

void Segment::setZ1(double n)
{
    this->z1 = n;
    setDefault();
}

void Segment::setZ2(double n)
{
    this->z2 = n;
    setDefault();
}

void Segment::setDefault()
{
    pointsIn.clear();
    pointsOut.clear();
    pointsOut.push_back(Point(x1, y1, z1));
    pointsOut.push_back(Point(x2, y2, z2));
}

void Segment::setPointsIn(std::vector<Point> points)
{
    this->pointsIn = points;
}

void Segment::setPointsOut(std::vector<Point> points)
{
    this->pointsOut = points;
}

void Segment::changePoints()
{
    std::swap(this->x1, this->x2);
    std::swap(this->y1, this->y2);
    std::swap(this->z1, this->z2);
    setDefault();
}
