#include <vector>

class Point
{
public:
    float x, y, z;
    Point (float, float, float);
    Point ();
};

class Cylinder
{
private:
    std::vector<std::vector<std::vector<Point>>> linesArr, fEllArr, sEllArr;
    std::vector<std::vector<std::vector<std::vector<Point>>>> normals;
    std::vector<std::vector<std::vector<std::vector<std::pair<float, float>>>>> textCoords;
    void countTextCoords(int, int);
public:
    float xCenter, yCenter, rx, ry, h, ddx, ddy;
    Cylinder (float, float, float, float, float, float, float);
    void recalc (int n, int k);
    std::vector<std::vector<std::vector<Point>>>& getLinesArr();
    std::vector<std::vector<std::vector<Point>>>& getfEllArr();
    std::vector<std::vector<std::vector<Point>>>& getsEllArr();
    std::vector<std::vector<std::vector<std::vector<Point>>>>& getNormals();
    std::vector<std::vector<std::vector<std::vector<std::pair<float, float>>>>>& getTextCoords();
};

class CutCylinder
{
private:
    std::vector<std::vector<std::vector<Point>>> linesArr, fEllArr, sEllArr;
    std::vector<std::vector<std::vector<std::vector<Point>>>> normals;
public:
    float xCenter, yCenter, rxU, ryU, rxD, ryD, height, ddx, ddy;
    CutCylinder (float, float, float, float, float, float, float);
    void recalc (int n, int k);
    std::vector<std::vector<std::vector<Point>>>& getfEllArr ();
    std::vector<std::vector<std::vector<Point>>>& getsEllArr ();
    std::vector<std::vector<std::vector<Point>>>& getLinesArr ();
    std::vector<std::vector<std::vector<std::vector<Point>>>>& getNormals();
};

typedef struct s_stage
{
    float rotationX, rotationY, rotationZ;
    int n, k;
    bool reDraw, LightModel, lighted, animation, animated, textured;
} t_stage;

Point multiVector (Point, Point);
Point countNormal (int, int, int, int,
                   std::vector<std::vector<std::vector<std::vector<Point>>>>*);
float dot (Point, Point);
void normalize (Point*);
void countNormals (std::vector<std::vector<std::vector<Point>>>*,
                   std::vector<std::vector<std::vector<Point>>>*,
                   std::vector<std::vector<std::vector<Point>>>*,
                   std::vector<std::vector<std::vector<std::vector<Point>>>>*,
                   float, float, float, float, float);
