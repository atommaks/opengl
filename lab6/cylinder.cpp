#include <math.h>
#include "./cylinder.hpp"

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

std::vector<std::vector<std::vector<std::vector<Point> > > >& Cylinder::getNormals ()
{
    return this->normals;
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
            curH += hStep;
            x += ddxStep;
            y += ddyStep;
            sector.push_back(block);
        }
        linesArr.push_back(sector);
        curAngle += step;
    }
    countNormals(&fEllArr, &sEllArr, &linesArr, &normals, ddx, ddy, h, xCenter, yCenter);
    countTextCoords(n, k);
}

CutCylinder::CutCylinder (double xCenter, double yCenter, double rxU, double ryU, double rxD, double ryD, double h)
{
    this->xCenter = xCenter; this->yCenter = yCenter;
    this->rxU = rxU; this->rxD = rxD;
    this->ryU = ryU; this->ryD = ryD;
    this->height = h;
    ddx = rxD - rxU, ddy = ryD - ryU;
}

std::vector<std::vector<std::vector<Point> > >& CutCylinder::getfEllArr ()
{
    return this->fEllArr;
}

std::vector<std::vector<std::vector<Point> > >& CutCylinder::getsEllArr ()
{
    return this->sEllArr;
}

std::vector<std::vector<std::vector<Point> > >& CutCylinder::getLinesArr ()
{
    return this->linesArr;
}

std::vector<std::vector<std::vector<std::vector<Point> > > >& CutCylinder::getNormals ()
{
    return this->normals;
}

void CutCylinder::recalc(int n, int k)
{
    fEllArr.clear();
    sEllArr.clear();
    double rxUpStep = rxU / k, ryUpStep = ryU / k;
    double rxDownStep = rxD / k, ryDownStep = ryD / k;
    double step = (double)360 / n;
    double curAngle = 0.0;
    for (int i = 0; i < n; i++)
    {
        std::vector<std::vector<Point>> upSector, downSector;
        double rxdi = rxDownStep, rxui = rxUpStep;
        double rydi = ryDownStep, ryui = ryUpStep;
        for (int j = 0; j < k; j++)
        {
            std::vector<Point> dBlock, uBlock;
            double fAngle = double(curAngle * M_PI) / 180;
            double sAngle = (i == n - 1) ? 0.0 : double((curAngle + step) * M_PI / 180);
            double dxd1 = rxdi * cos(fAngle), dyd1 = rydi * sin(fAngle);
            double dxu1 = rxui * cos(fAngle), dyu1 = ryui * sin(fAngle);
            double dxd2 = rxdi * cos(sAngle), dyd2 = rydi * sin(sAngle);
            double dxu2 = rxui * cos(sAngle), dyu2 = ryui * sin(sAngle);
            dBlock.push_back(Point(xCenter, 0, yCenter));
            dBlock.push_back(Point(xCenter + dxd1, 0, yCenter + dyd1));
            dBlock.push_back(Point(xCenter + dxd2, 0, yCenter + dyd2));
            uBlock.push_back(Point(xCenter, height, yCenter));
            uBlock.push_back(Point(xCenter + dxu2, height, yCenter + dyu2));
            uBlock.push_back(Point(xCenter + dxu1, height, yCenter + dyu1));
            upSector.push_back(uBlock);
            downSector.push_back(dBlock);
            rxdi += rxDownStep, rydi += ryDownStep;
            rxui += rxUpStep, ryui += ryUpStep;
        }
        curAngle += step;
        fEllArr.push_back(downSector);
        sEllArr.push_back(upSector);
    }
    
    linesArr.clear();
    curAngle = 0.0;
    for (int i = 0; i < n; i++)
    {
        std::vector<std::vector<Point>> sector;
        double fAngle = double(curAngle * M_PI) / 180;
        double sAngle = (i == n - 1) ? 0.0 : double((curAngle + step) * M_PI / 180);
        double x = rxD, y = ryD;
        double hStep = height / k, ddxStep = ddx / k, ddyStep = ddy / k;
        double curH = 0.0;
        for (int j = 0; j < k; j++)
        {
            std::vector<Point> block;
            block.push_back(Point(xCenter + x * cos(fAngle), curH, yCenter + y * sin(fAngle)));
            block.push_back(Point(xCenter + (x - ddxStep) * cos(fAngle), curH + hStep, yCenter + (y - ddyStep) * sin(fAngle)));
            block.push_back(Point(xCenter + (x - ddxStep) * cos(sAngle), curH + hStep, yCenter + (y - ddyStep) * sin(sAngle)));
            block.push_back(Point(xCenter + x * cos(sAngle), curH, yCenter + y * sin(sAngle)));
            sector.push_back(block);
            curH += hStep;
            x -= ddxStep;
            y -= ddyStep;
        }
        linesArr.push_back(sector);
        curAngle += step;
    }
    countNormals(&fEllArr, &sEllArr, &linesArr, &normals, 0, 0, height, xCenter, yCenter);
    //countTextCoords(n, k);
}

std::vector<std::vector<std::vector<std::vector<std::pair<double, double>>>>>& Cylinder::getTextCoords()
{
    return this->textCoords;
}

//std::vector<std::vector<std::vector<std::vector<std::pair<double, double>>>>>& CutCylinder::getTextCoords()
//{
//    return this->textCoords;
//}

void Cylinder::countTextCoords(int n, int k)
{
    textCoords.clear();
    double y = 0.5 * ry / rx;
    double rxStep = 0.5 / k, ryStep = y / k;
    double rxi = rxStep, ryi = ryStep;
    double step = 360.0 / n;
    double curAngle = 0.0;
    std::vector<std::vector<std::vector<std::pair<double, double>>>> arr1, arr2, arr3;
    for (int i = 0; i < n; i++)
    {
        std::vector<std::vector<std::pair<double, double>>> fSector, sSector;
        rxi = rxStep, ryi = ryStep;
        for (int j = 0; j < k; j++)
        {
            std::vector<std::pair<double, double>> fBlock, sBlock;
            double fAngle = double(curAngle * M_PI) / 180;
            double sAngle = (i == n - 1) ? 0.f : double((curAngle + step) * M_PI / 180);
            double dx1 = rxi * cos(fAngle), dy1 = ryi * sin(fAngle);
            double dx2 = rxi * cos(sAngle), dy2 = ryi * sin(sAngle);
            fBlock.push_back(std::pair<double, double>(0.5, 0.5));
            fBlock.push_back(std::pair<double, double>(0.5 + dx1, 0.5 + dy1));
            fBlock.push_back(std::pair<double, double>(0.5 + dx2, 0.5 + dy2));
            sBlock.push_back(std::pair<double, double>(0.5, 0.5));
            sBlock.push_back(std::pair<double, double>(0.5 + dx2, 0.5 + dy2));
            sBlock.push_back(std::pair<double, double>(0.5 + dx1, 0.5 + dy1));
            fSector.push_back(fBlock);
            sSector.push_back(sBlock);
            rxi += rxStep, ryi += ryStep;
        }
        curAngle += step;
        arr1.push_back(fSector);
        arr2.push_back(sSector);
    }
    textCoords.push_back(arr1);
    textCoords.push_back(arr2);
    
    double hStep = 1.0 / k, lenStep = 1.0 / n;
    double curLen = 0.0;
    for (int i = 0; i < n; i++)
    {
        std::vector<std::vector<std::pair<double, double>>> sector;
        double curH = 0.0;
        for (int j = 0; j < k; j++)
        {
            std::vector<std::pair<double, double>> block;
            block.push_back(std::pair<double, double>(curLen, curH));
            block.push_back(std::pair<double, double>(curLen, curH + hStep));
            block.push_back(std::pair<double, double>(curLen + lenStep, curH + hStep));
            block.push_back(std::pair<double, double>(curLen + lenStep, curH));
            curH += hStep;
            curLen += lenStep;
            sector.push_back(block);
        }
        arr3.push_back(sector);
    }
    textCoords.push_back(arr3);
}

//void CutCylinder::countTextCoords(int n, int k)
//{
//
//}

Point multiVector (Point a, Point b)
{
    double A = a.y * b.z - b.y * a.z;
    double B = a.x * b.z - b.x * a.z;
    double C = a.x * b.y - b.x * a.y;
    return Point(A, B, C);
}

void normalize (Point* p)
{
    double wrki = sqrt(pow(p->x, 2) + pow(p->y, 2) + pow(p->z, 2));
    p->x = p->x / wrki;
    p->y = p->y / wrki;
    p->z = p->z / wrki;
}

double dot (Point a, Point b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void countNormals (std::vector<std::vector<std::vector<Point>>>* fEllArr,
                   std::vector<std::vector<std::vector<Point>>>* sEllArr,
                   std::vector<std::vector<std::vector<Point>>>* linesArr,
                   std::vector<std::vector<std::vector<std::vector<Point>>>>* normals,
                   double q, double w, double e, double xCenter, double yCenter)
{
    normals->clear();
    std::vector<std::vector<std::vector<Point>>> *arr;
    std::vector<std::vector<std::vector<std::vector<Point>>>> tNormals;
    for (int a = 0; a < 3; a++)
    {
        if (a == 0)
            arr = fEllArr;
        else if (a == 1)
            arr = sEllArr;
        else
            arr = linesArr;
        std::vector<std::vector<std::vector<Point>>> part;
        for (int i = 0; i < arr->size(); i++)
        {
            std::vector<std::vector<Point>> sector;
            for (int j = 0; j < (*arr)[i].size(); j++)
            {
                std::vector<Point> block;
                Point A, B;
                if (a > 1)
                {
                    A = Point((*arr)[i][j][0].x - (*arr)[i][j][1].x, (*arr)[i][j][0].y - (*arr)[i][j][1].y, (*arr)[i][j][0].z - (*arr)[i][j][1].z);
                    B = Point((*arr)[i][j][1].x - (*arr)[i][j][2].x, (*arr)[i][j][1].y - (*arr)[i][j][2].y, (*arr)[i][j][1].z - (*arr)[i][j][2].z);
                }
                else
                {
                    A = Point((*arr)[i][j][1].x - (*arr)[i][j][0].x, (*arr)[i][j][1].y - (*arr)[i][j][0].y, (*arr)[i][j][1].z - (*arr)[i][j][0].z);
                    B = Point((*arr)[i][j][1].x - (*arr)[i][j][2].x, (*arr)[i][j][1].y - (*arr)[i][j][2].y, (*arr)[i][j][1].z - (*arr)[i][j][2].z);
                }
                Point normal = multiVector(A, B);
                normalize(&normal);
                for (int c = 0; c < (*arr)[i][j].size(); c++)
                    block.push_back(normal);
                sector.push_back(block);
            }
            part.push_back(sector);
        }
        tNormals.push_back(part);
    }
    
    for (int a = 0; a < 3; a++)
    {
        std::vector<std::vector<std::vector<Point> > > part;
        for (int i = 0; i < tNormals[a].size(); i++)
        {
            std::vector<std::vector<Point>> sector;
            for (int j = 0; j < tNormals[a][i].size(); j++)
            {
                std::vector<Point> block;
                for (int m = 0; m < tNormals[a][i][j].size(); m++)
                    block.push_back(countNormal(a, i, j, m, &tNormals));
                sector.push_back(block);
            }
            part.push_back(sector);
        }
        normals->push_back(part);
    }
}

Point countNormal (int a, int i, int j, int k,
                   std::vector<std::vector<std::vector<std::vector<Point>>>>* tNormals)
{
    if ((a == 0 || a == 1) && (k == 0 || j < (*tNormals)[a][i].size() - 1))
        return (*tNormals)[a][i][j][k];
    
    int size = (*tNormals)[a].size();
    int index1 = (i == 0) ? size - 1 : i - 1;
    int index2 = (i == size - 1) ? 0 : i + 1;
    Point normal;
    if (a == 0)
    {
        if (k == 1)
        {
            normal.x = ((*tNormals)[a][i][j][k].x + (*tNormals)[2][i][j][3].x +
                        (*tNormals)[2][index1][j][0].x + (*tNormals)[a][index1][j][2].x) / 4.0;
            normal.y = ((*tNormals)[a][i][j][k].y + (*tNormals)[2][i][j][3].y +
                        (*tNormals)[2][index1][j][0].y + (*tNormals)[a][index1][j][2].y) / 4.0;
            normal.z = ((*tNormals)[a][i][j][k].z + (*tNormals)[2][i][j][3].z +
                        (*tNormals)[2][index1][j][0].z + (*tNormals)[a][index1][j][2].z) / 4.0;
            return normal;
        }
        if (k == 2)
        {
            normal.x = ((*tNormals)[a][i][j][k].x + (*tNormals)[2][i][j][0].x +
                        (*tNormals)[2][index2][j][3].x + (*tNormals)[a][index2][j][1].x) / 4.0;
            normal.y = ((*tNormals)[a][i][j][k].y + (*tNormals)[2][i][j][0].y +
                        (*tNormals)[2][index2][j][3].y + (*tNormals)[a][index2][j][1].y) / 4.0;
            normal.z = ((*tNormals)[a][i][j][k].z + (*tNormals)[2][i][j][0].z +
                        (*tNormals)[2][index2][j][3].z + (*tNormals)[a][index2][j][1].z) / 4.0;
            return normal;
        }
    }
    
    if (a == 1)
    {
        if (k == 1)
        {
            normal.x = ((*tNormals)[a][i][j][k].x + (*tNormals)[2][i][j][k].x +
                        (*tNormals)[2][index2][j][2].x + (*tNormals)[a][index2][j][2].x) / 4.0;
            normal.y = ((*tNormals)[a][i][j][k].y + (*tNormals)[2][i][j][k].y +
                        (*tNormals)[2][index2][j][2].y + (*tNormals)[a][index2][j][2].y) / 4.0;
            normal.z = ((*tNormals)[a][i][j][k].z + (*tNormals)[2][i][j][k].z +
                        (*tNormals)[2][index2][j][2].z + (*tNormals)[a][index2][j][2].z) / 4.0;
            return normal;
        }
        if (k == 2)
        {
            normal.x = ((*tNormals)[a][i][j][k].x + (*tNormals)[2][i][j][k].x +
                        (*tNormals)[2][index1][j][1].x + (*tNormals)[a][index1][j][1].x) / 4.0;
            normal.y = ((*tNormals)[a][i][j][k].y + (*tNormals)[2][i][j][k].y +
                        (*tNormals)[2][index1][j][1].y + (*tNormals)[a][index1][j][1].y) / 4.0;
            normal.z = ((*tNormals)[a][i][j][k].z + (*tNormals)[2][i][j][k].z +
                        (*tNormals)[2][index1][j][1].z + (*tNormals)[a][index1][j][1].z) / 4.0;
            return normal;
        }
    }
    
    if (j == 0 && (k == 0 || k == 3))
    {
        if (k == 0)
        {
            normal.x = ((*tNormals)[a][i][j][k].x + (*tNormals)[a][index2][j][3].x +
                        (*tNormals)[0][i][j][2].x + (*tNormals)[0][index2][j][1].x) / 4.0;
            normal.y = ((*tNormals)[a][i][j][k].y + (*tNormals)[a][index2][j][3].y +
                        (*tNormals)[0][i][j][2].y + (*tNormals)[0][index2][j][1].y) / 4.0;
            normal.z = ((*tNormals)[a][i][j][k].z + (*tNormals)[a][index2][j][3].z +
                        (*tNormals)[0][i][j][2].z + (*tNormals)[0][index2][j][1].z) / 4.0;
            return normal;
        }
        if (k == 3)
        {
            normal.x = ((*tNormals)[a][i][j][k].x + (*tNormals)[a][index1][j][0].x +
                        (*tNormals)[0][i][j][1].x + (*tNormals)[0][index1][j][2].x / 4.0);
            normal.y = ((*tNormals)[a][i][j][k].y + (*tNormals)[a][index1][j][0].y +
                        (*tNormals)[0][i][j][1].y + (*tNormals)[0][index1][j][2].y / 4.0);
            normal.z = ((*tNormals)[a][i][j][k].z + (*tNormals)[a][index1][j][0].z +
                        (*tNormals)[0][i][j][1].z + (*tNormals)[0][index1][j][2].z / 4.0);
            return normal;
        }
    }
    if ((j == (*tNormals)[a][i % size].size() - 1) && (k == 1 || k == 2))
    {
        if (k == 1)
        {
            normal.x = ((*tNormals)[a][i][j][k].x +
                        (*tNormals)[a][index2][j][2].x + (*tNormals)[1][i][j][k].x +
                        (*tNormals)[1][index2][j][2].x) / 4.0;
            normal.y = ((*tNormals)[a][i][j][k].y +
                        (*tNormals)[a][index2][j][2].y + (*tNormals)[1][i][j][k].y +
                        (*tNormals)[1][index2][j][2].y) / 4.0;
            normal.z = ((*tNormals)[a][i][j][k].z +
                        (*tNormals)[a][index2][j][2].z + (*tNormals)[1][i][j][k].z +
                        (*tNormals)[1][index2][j][2].z) / 4.0;
            return normal;
        }
        if (k == 2)
        {
            normal.x = ((*tNormals)[a][i][j][k].x + (*tNormals)[a][index1][j][1].x +
                        (*tNormals)[1][i][j][k].x + (*tNormals)[1][index1][j][1].x) / 4.0;
            normal.y = ((*tNormals)[a][i][j][k].y + (*tNormals)[a][index1][j][1].y +
                        (*tNormals)[1][i][j][k].y + (*tNormals)[1][index1][j][1].y) / 4.0;
            normal.z = ((*tNormals)[a][i][j][k].z + (*tNormals)[a][index1][j][1].z +
                        (*tNormals)[1][i][j][k].z + (*tNormals)[1][index1][j][1].z) / 4.0;
            return normal;
        }
    }
    
    if (k == 0 || k == 1)
    {
        normal.x = ((*tNormals)[a][i][j][0].x + (*tNormals)[a][index2][j][3].x) / 4.0;
        normal.y = ((*tNormals)[a][i][j][0].y + (*tNormals)[a][index2][j][3].y) / 4.0;
        normal.z = ((*tNormals)[a][i][j][0].z + (*tNormals)[a][index2][j][3].z) / 4.0;
        return normal;
    }
    
    normal.x = ((*tNormals)[a][i][j][3].x + (*tNormals)[a][index1][j][0].x) / 2.0;
    normal.y = ((*tNormals)[a][i][j][3].y + (*tNormals)[a][index1][j][0].y) / 2.0;
    normal.z = ((*tNormals)[a][i][j][3].z + (*tNormals)[a][index1][j][0].z) / 2.0;
    return normal;
}

