#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "./geometry.cpp"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

void keyCallback (GLFWwindow*, int, int, int, int);
void drawCylinder (Cylinder* cylinder);
void drawLine (Segment*);
void cut (Cylinder*, Segment*);

Segment* segment;
Cylinder* c;
bool reDraw = false;
int n = 10, k = 1, screenWidth = 1280, screenHeight = 720, sign = 1;
double rotationY = 0.f, rotationX = 0.f, rotationZ = 0.f;

int main (int argc, char** argv)
{
    GLFWwindow *window;
    
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lab5", nullptr, nullptr);
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);
    
    double a = asin(tanf(M_PI / 6.f)), b = M_PI_4, ratio = (double) screenWidth / screenHeight;
    double m[16] =
    {
        cos(b) / ratio, sin(b) * sin(a), 0.f, 0.f,
        0.f, cos(a), 0.f, 0.f,
        sin(b) / ratio, -sin(a) * cos(b), 0.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
    
    //segment = new Segment (0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f);
    segment = new Segment (-0.25, 0.15, 0.0, 0.25, 0.15, 0.0);
    c = new Cylinder(0.0f, 0.0f, 0.25f, 0.25f, 0.3f, 0.f, 0.0f);
    c->recalc(n, k);
    while (!glfwWindowShouldClose(window))
    {
        glViewport(0.f, 0.f, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glLoadMatrixd(m);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glRotated(rotationX, 1, 0, 0);
        glRotated(rotationY, 0, 1, 0);
        glRotated(rotationZ, 0, 0, 1);
        drawLine(segment);
        drawCylinder(c);
        glPopMatrix();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    delete c;
    delete segment;
    glfwTerminate();
    return 0;
}

Point multiVector (Point a, Point b)
{
    double A = a.y * b.z - b.y * a.z;
    double B = a.x * b.z - b.x * a.z;
    double C = a.x * b.y - b.x * a.y;
    return Point(A, B, C);
}

double dot (Point a, Point b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void cut (Cylinder* c, Segment* s)
{
    std::vector<std::vector<Point>> edges;
    auto arr = c->getfEllArr();
    edges.push_back(arr[0][0]);
    arr = c->getLinesArr();
    for (int i = 0; i < arr.size(); i++)
        edges.push_back(arr[i][0]);
    arr = c->getsEllArr();
    edges.push_back(arr[0][0]);
    //counting normals for each edge
    std::vector<Point> normals;
    for (int i = 0; i < edges.size(); i++)
    {
        Point a = Point(edges[i][0].x - edges[i][1].x, edges[i][0].y - edges[i][1].y, edges[i][0].z - edges[i][1].z);
        Point b = Point(edges[i][1].x - edges[i][2].x, edges[i][1].y - edges[i][2].y, edges[i][1].z - edges[i][2].z);
        Point normal = multiVector(a, b);
        Point t;
        if (i == edges.size() - 1)
            t = Point(-c->ddx, -c->ddy, -c->h);
        else if (i == 0)
            t = Point(c->ddx, c->ddy, c->h);
        else
            t = Point(c->xCenter - edges[i][0].x, c->yCenter - edges[i][0].y, -edges[i][0].z);
        if (dot(normal, t) < 0)
        {
            normal.x *= -1;
            normal.y *= -1;
            normal.z *= -1;
        }
        normals.push_back(normal);
    }
    //cyrus-beck algorithm
    Point D = Point(s->getX2() - s->getX1(), s->getY2() - s->getY1(), s->getZ2() - s->getZ1());
    double tl = 0.f, th = 1.f;
    for (int i = 0; i < edges.size(); i++)
    {
        Point W = Point(s->getX1() - edges[i][0].x, s->getY1() - edges[i][0].y, s->getZ1() - edges[i][0].z);
        double multiD = dot(D, normals[i]);
        double multiW = dot(W, normals[i]);
        if (multiD != 0)
        {
            double t = -(multiW / multiD);
            if (multiD > 0)
            {
                if (t <= 1)
                    tl = std::max(tl, t);
            }
            else
            {
                if (t >= 0)
                    th = std::min(th, t);
            }
        }
    }

    if (tl <= th)
    {
        std::vector<Point> in;
        Point p1, p2;
        p1.x = D.x * tl + s->getX1(); p1.y = D.y * tl + s->getY1(); p1.z = D.z * tl + s->getZ1();
        p2.x = D.x * th + s->getX1(); p2.y = D.y * th + s->getY1(); p2.z = D.z * th + s->getZ1();
        in.push_back(p1);
        in.push_back(p2);
        s->setPointsIn(in);
    }
}

void drawLine (Segment* s)
{
    glColor3ub(255, 255, 0);
    glBegin(GL_LINES);
    for (auto p = s->getPointsOut().begin(); p != s->getPointsOut().end(); p++)
        glVertex3f(p->x, p->y, p->z);
    glEnd();
    glColor3ub(255, 0, 255);
    glBegin(GL_LINES);
    for (auto p = s->getPointsIn().begin(); p != s->getPointsIn().end(); p++)
        glVertex3f(p->x, p->y, p->z);
    glEnd();
}

void drawCylinder (Cylinder* cylinder)
{
    if (reDraw)
    {
        cylinder->recalc(n, k);
        reDraw = false;
    }
    auto mode = GL_LINE_LOOP;
    glColor3ub(255, 255, 255);
    std::vector<std::vector<std::vector<Point> > > arr;
    for (int a = 0; a < 3; a++)
    {
        if (a == 0)
            arr = cylinder->getfEllArr();
        else if (a == 1)
            arr = cylinder->getsEllArr();
        else
            arr = cylinder->getLinesArr();
        for (int i = 0; i < arr.size(); i++)
        {
            for (int j = 0; j < arr[i].size(); j++)
            {
                glBegin(mode);
                for (int c = 0; c < arr[i][j].size(); c++)
                    glVertex3f(arr[i][j][c].x, arr[i][j][c].y, arr[i][j][c].z);
                glEnd();
            }
        }
    }
}

void keyCallback (GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        const double translationSpeed = 0.05f, rotationSpeed = 1.5;
        switch (key)
        {
            case GLFW_KEY_LEFT_SHIFT:
                sign = -sign;
                break;
            case GLFW_KEY_1:
                if (n > 3)
                {
                    n--;
                    reDraw = true;
                }
                break;
            case GLFW_KEY_2:
                if (n < 360)
                {
                    n++;
                    reDraw = true;
                }
                break;
            case GLFW_KEY_3:
                if (k > 1)
                {
                    k--;
                    reDraw = true;
                }
                break;
            case GLFW_KEY_4:
                if (k < 20)
                {
                    k++;
                    reDraw = true;
                }
                break;
            case GLFW_KEY_Q:
                segment->setX1(segment->getX1() + (sign * translationSpeed));
                break;
            case GLFW_KEY_W:
                segment->setX2(segment->getX2() + (sign * translationSpeed));
                break;
            case GLFW_KEY_E:
                segment->setY1(segment->getY1() + (sign * translationSpeed));
                break;
            case GLFW_KEY_R:
                segment->setY2(segment->getY2() + (sign * translationSpeed));
                break;
            case GLFW_KEY_T:
                segment->setZ1(segment->getZ1() + (sign * translationSpeed));
                break;
            case GLFW_KEY_Y:
                segment->setZ2(segment->getZ2() + (sign * translationSpeed));
                break;
            case GLFW_KEY_ENTER:
                segment->setDefault();
                cut(c, segment);
                break;
            case GLFW_KEY_LEFT:
                rotationY -= rotationSpeed;
                break;
            case GLFW_KEY_RIGHT:
                rotationY += rotationSpeed;
                break;
            case GLFW_KEY_UP:
                rotationX += rotationSpeed;
                break;
            case GLFW_KEY_DOWN:
                rotationX -= rotationSpeed;
                break;
            case GLFW_KEY_O:
                rotationZ -= rotationSpeed;
                break;
            case GLFW_KEY_P:
                rotationZ += rotationSpeed;
                break;
        }
    }
}
