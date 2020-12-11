#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <math.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

static void keyCallback (GLFWwindow*, int, int, int, int);
static void mouseButtonCallback (GLFWwindow*, int, int, int);
static void windowsizeCallback (GLFWwindow*, int, int);
void modBresenham ();
void bresenham (int, int, int, int);
void clear ();
void fill ();
void filter ();
void setPixel (int, int, float, float, float, float*);
bool isColored (int, int);
bool isOneSidedEdges (int);
float topLeft (int, int, int);
float top (int, int, int);
float topRight (int, int, int);
float left (int, int, int);
float center (int, int, int);
float right (int, int, int);
float bottomLeft (int, int, int);
float bottom (int, int, int);
float bottomRight (int, int, int);

std::vector<std::pair<int, int>> points;
float *buf, *fbuf;
double xPos = 0, yPos = 0;
bool filtered = false, filled = false;
int screenWidth = 1280, screenHeight = 720;

int main (int argc, char **argv)
{
    GLFWwindow *window;
    
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lab4", nullptr, nullptr);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetWindowSizeCallback(window, windowsizeCallback);
    glfwMakeContextCurrent(window);
    glfwSetWindowSize(window, 1280, 720);

    buf = new float[screenHeight * screenWidth * 3];
    fbuf = new float[screenHeight * screenWidth * 3];
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    while (!glfwWindowShouldClose(window))
    {
        glfwGetWindowSize(window, &screenWidth, &screenHeight);
        glViewport(0.f, 0.f, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, screenWidth, 0, -screenHeight, -1, 1);

        glfwGetCursorPos(window, &xPos, &yPos);
        //glRasterPos2i(0, 0);
        if (filtered)
            glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, fbuf);
        else
            glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, buf);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    delete[] buf;
    delete[] fbuf;
    
    return 0;
}

void bresenham (int x1, int y1, int x2, int y2)
{
    int xLen = abs(x2 - x1), yLen = abs(y2 - y1);
    bool h = false;
    if (xLen < yLen)
    {
        h = true;
        int tmp = x1;
        x1 = y1;
        y1 = tmp;
        tmp = x2;
        x2 = y2;
        y2 = tmp;
        tmp = xLen;
        xLen = yLen;
        yLen = tmp;
        
    }
    
    int e = -xLen;
    int de = 2 * yLen;
    int y = y1;
    int dx = x1 > x2 ? -1 : 1;
    int dy = y1 > y2 ? -1 : 1;
    for (int x = x1; dx * x <= dx * x2; x += dx)
    {
        if (!h)
            setPixel(y, x, 1, 0, 0, buf);
        else
            setPixel(x, y, 1, 0, 0, buf);
        e += de;
        if (e >= 0)
        {
            y += dy;
            e -= 2 * xLen;
        }
    }
}

void modBresenham ()
{
    for (int i = 1; i <= points.size(); i++)
    {
        auto p1 = points[i - 1];
        auto p2 = points[i % points.size()];
        
        if (p1.second == p2.second){
//            if (abs(p2.first - p1.first) % 2 != 0)
//                setPixel(p2.second, p2.first + 1, 1, 0, 0, buf);
            continue;
        }
        
        int x1 = p1.first, y1 = p1.second;
        int x2 = p2.first, y2 = p2.second;
        int xLen = abs(x2 - x1), yLen = abs(y2 - y1);
        
        bool h = false;
        if (xLen < yLen)
        {
            h = true;
            int tmp = x1;
            x1 = y1;
            y1 = tmp;
            tmp = x2;
            x2 = y2;
            y2 = tmp;
            tmp = xLen;
            xLen = yLen;
            yLen = tmp;
        }
        
        int dx = x1 > x2 ? -1 : 1;
        int dy = y1 > y2 ? -1 : 1;
        int e = -xLen, de = 2 * yLen;
        int y = y1, x = x1;
        
        if (!isOneSidedEdges(i - 1))
        {
            if (!h)
                setPixel(y, x, 1, 0, 0, buf);
            else
                setPixel(x, y, 1, 0, 0, buf);
        }
        
        for (x = x1; dx * x < dx * x2; x += dx)
        {
            if (h && x != x1)
            {
                if (isColored(x, y))
                    setPixel(x, y, 0, 0, 0, buf);
                else
                    setPixel(x, y, 1, 0, 0, buf);
            }
            e += de;
            if (e >= 0) {
                if (!h && x != x1)
                {
                    if (y != y1)
                    {
                        if (isColored(y, x))
                            setPixel(y, x, 0, 0, 0, buf);
                        else
                            setPixel(y, x, 1, 0, 0, buf);
                    }
                }
                y += dy;
                e -= 2 * xLen;
            }
        }
    }
}

void fill ()
{
    for (int i = 0; i < screenHeight; i++)
        for (int j = 0; j < screenWidth; j++)
            setPixel(i, j, 0, 0, 0, buf);
    modBresenham();
    for (int i = 0; i < screenHeight; i++)
    {
        bool f = false;
        for (int j = 0; j < screenWidth; j++)
        {
            if (isColored(i, j))
                f = !f;
            else if (f)
                setPixel(i, j, 1, 0, 0, buf);
        }
    }
    filled = true;
}

bool isColored (int i, int j)
{
    return buf[i * screenWidth * 3 + j * 3 + 0] == 1 &&
           buf[i * screenWidth * 3 + j * 3 + 1] == 0 &&
           buf[i * screenWidth * 3 + j * 3 + 2] == 0;
}

bool isOneSidedEdges (int index)
{
    int y = points[index].second;
    if (index == 0)
    {
        return (points[points.size() - 1].second <= y && points[1].second <= y) ||
               (points[points.size() - 1].second >= y && points[1].second >= y);
    }
    else if (index == points.size() - 1)
    {
        return (points[index - 1].second <= y && points[0].second <= y) ||
               (points[index - 1].second >= y && points[0].second >= y);
    }
    else
    {
        return (points[index - 1].second <= y && points[index + 1].second <= y) ||
               (points[index - 1].second >= y && points[index + 1].second >= y);
    }
}

void setPixel (int i, int j, float R, float G, float B, float *arr)
{
    arr[i * screenWidth * 3 + j * 3 + 0] = R;
    arr[i * screenWidth * 3 + j * 3 + 1] = G;
    arr[i * screenWidth * 3 + j * 3 + 2] = B;
}

void clear ()
{
    points.clear();
    for (int i = 0; i < screenHeight; i++)
    {
        for (int j = 0; j < screenWidth; j++)
        {
            setPixel(i, j, 0, 0, 0, buf);
            setPixel(i, j, 0, 0, 0, fbuf);
        }
    }
    filled = false;
    filtered = false;
}

float topLeft (int i, int j, int k)
{
    return buf[(i * screenWidth + j - screenWidth - 1) * 3 + k];
}

float top (int i, int j, int k)
{
    return buf[(i * screenWidth + j - screenWidth) * 3 + k];
}

float topRight (int i, int j, int k)
{
    return buf[(i * screenWidth + j - screenWidth + 1) * 3 + k];
}

float left (int i, int j, int k)
{
    return buf[(i * screenWidth + j - 1) * 3 + k];
}

float center (int i, int j, int k)
{
    return buf[(i * screenWidth + j) * 3 + k];
}

float right (int i, int j, int k)
{
    return buf[(i * screenWidth + j + 1) * 3 + k];
}

float bottomLeft (int i, int j, int k)
{
    return buf[(i * screenWidth + j + screenWidth - 1) * 3 + k];
}

float bottom (int i, int j, int k)
{
    return buf[(i * screenWidth + j + screenWidth) * 3 + k];
}

float bottomRight (int i, int j, int k)
{
    return buf[(i * screenWidth + j + screenWidth + 1) * 3 + k];
}

void filter ()
{
//    int mask[3][3] = {{0, 1, 0}, {1, 2, 1}, {0, 1, 0}};
//    int mask[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
    int mask[3][3] = {{2, 4, 2}, {4, 8, 4}, {2, 4, 2}};
    for (int i = 0; i < screenHeight; i++) {
        for (int j = 0; j < screenWidth; j++) {
            if (i == 0) {
                if (j == 0) {
                    float r, g, b;
                    int weight = mask[1][1] + mask[1][2] + mask[2][1] + mask[2][2];
                    r = center(i, j, 0) * mask[1][1] + right(i, j, 0) * mask[1][2] +
                        bottom(i, j, 0) * mask[2][1] + bottomRight(i, j, 0) * mask[2][2];
                    g = center(i, j, 1) * mask[1][1] + right(i, j, 1) * mask[1][2] +
                        bottom(i, j, 1) * mask[2][1] + bottomRight(i, j, 1) * mask[2][2];
                    b = center(i, j, 2) * mask[1][1] + right(i, j, 2) * mask[1][2] +
                        bottom(i, j, 2) * mask[2][1] + bottomRight(i, j, 2) * mask[2][2];
                    setPixel(i, j, r / weight, g / weight, b / weight, fbuf);
                } else if (j == screenWidth - 1) {
                    float r, g, b;
                    int weight = mask[1][1] + mask[1][0] + mask[2][1] + mask[2][0];
                    r = center(i, j, 0) * mask[1][1] + left(i, j, 0) * mask[1][0] +
                        bottom(i, j, 0) * mask[2][1] + bottomLeft(i, j, 0) * mask[2][0];
                    g = center(i, j, 1) * mask[1][1] + left(i, j, 1) * mask[1][0] +
                        bottom(i, j, 1) * mask[2][1] + bottomLeft(i, j, 1) * mask[2][0];
                    b = center(i, j, 2) * mask[1][1] + left(i, j, 2) * mask[1][0] +
                        bottom(i, j, 2) * mask[2][1] + bottomLeft(i, j, 2) * mask[2][0];
                    setPixel(i, j, r / weight, g / weight, b / weight, fbuf);
                } else {
                    float r, g, b;
                    int weight = mask[1][0] + mask[1][1] + mask[1][2] + mask[2][0] + mask[2][1] + mask[2][2];
                    r = left(i, j, 0) * mask[1][0] + center(i, j, 0) * mask[1][1] +
                        right(i, j, 0) * mask[1][2] + bottomLeft(i, j, 0) * mask[2][0] +
                        bottom(i, j, 0) * mask[2][1] + bottomRight(i, j, 0) * mask[2][2];
                    g = left(i, j, 1) * mask[1][0] + center(i, j, 1) * mask[1][1] +
                        right(i, j, 1) * mask[1][2] + bottomLeft(i, j, 1) * mask[2][0] +
                        bottom(i, j, 1) * mask[2][1] + bottomRight(i, j, 1) * mask[2][2];
                    b = left(i, j, 2) * mask[1][0] + center(i, j, 2) * mask[1][1] +
                        right(i, j, 2) * mask[1][2] + bottomLeft(i, j, 2) * mask[2][0] +
                        bottom(i, j, 2) * mask[2][1] + bottomRight(i, j, 2) * mask[2][2];
                    setPixel(i, j, r / weight, g / weight, b / weight, fbuf);
                }
            } else if (i == screenHeight - 1) {
                if (j == 0) {
                    float r, g, b;
                    int weight = mask[0][1] + mask[0][2] + mask[1][1] + mask[1][2];
                    r = top(i, j, 0) * mask[0][1] + topRight(i, j, 0) * mask[0][2] +
                        center(i, j, 0) * mask[1][1] + right(i, j, 0) * mask[1][2];
                    g = top(i, j, 1) * mask[0][1] + topRight(i, j, 1) * mask[0][2] +
                        center(i, j, 1) * mask[1][1] + right(i, j, 1) * mask[1][2];;
                    b = top(i, j, 2) * mask[0][1] + topRight(i, j, 2) * mask[0][2] +
                        center(i, j, 2) * mask[1][1] + right(i, j, 2) * mask[1][2];;
                    setPixel(i, j, r / weight, g / weight, b / weight, fbuf);
                } else if (j == screenWidth - 1) {
                    float r, g, b;
                    int weight = mask[0][0] + mask[0][1] + mask[1][0] + mask[1][1];
                    r = topLeft(i, j, 0) * mask[0][0] + top(i, j, 0) * mask[0][1] +
                        left(i, j, 0) * mask[1][0] + center(i, j, 0) * mask[1][1];
                    g = topLeft(i, j, 1) * mask[0][0] + top(i, j, 1) * mask[0][1] +
                        left(i, j, 1) * mask[1][0] + center(i, j, 1) * mask[1][1];
                    b = topLeft(i, j, 2) * mask[0][0] + top(i, j, 2) * mask[0][1] +
                        left(i, j, 2) * mask[1][0] + center(i, j, 2) * mask[1][1];
                    setPixel(i, j, r / weight, g / weight, b / weight, fbuf);
                } else {
                    float r, g, b;
                    int weight = mask[0][0] + mask[0][1] + mask[0][2] + mask[1][0] + mask[1][1] + mask[1][2] + left(i, j, 0) * mask[1][0];
                    r = topLeft(i, j, 0) * mask[0][0] + top(i, j, 0) * mask[0][1] +
                        topRight(i, j, 0) * mask[0][2] + center(i, j, 0) * mask[1][1] +
                        right(i, j, 0) * mask[1][2];
                    g = topLeft(i, j, 1) * mask[0][0] + top(i, j, 1) * mask[0][1] +
                        topRight(i, j, 1) * mask[0][2] + center(i, j, 1) * mask[1][1] +
                        right(i, j, 1) * mask[1][2];
                    b = topLeft(i, j, 2) * mask[0][0] + top(i, j, 2) * mask[0][1] +
                        topRight(i, j, 2) * mask[0][2] + center(i, j, 2) * mask[1][1] +
                        right(i, j, 2) * mask[1][2];
                    setPixel(i, j, r / weight, g / weight, b / weight, fbuf);
                }
            } else {
                if (j == 0) {
                    float r, g, b;
                    int weight = mask[0][1] + mask[0][2] + mask[1][1] + mask[2][1] + mask[2][2] + mask[1][2];
                    r = top(i, j, 0) * mask[0][1] + topRight(i, j, 0) * mask[0][2] +
                        center(i, j, 0) * mask[1][1] + right(i, j, 0) * mask[1][2] +
                        bottom(i, j, 0) * mask[2][1] + bottomRight(i, j, 0) * mask[2][2];
                    g = top(i, j, 1) * mask[0][1] + topRight(i, j, 1) * mask[0][2] +
                        center(i, j, 1) * mask[1][1] + right(i, j, 1) * mask[1][2] +
                        bottom(i, j, 1) * mask[2][1] + bottomRight(i, j, 1) * mask[2][2];
                    b = top(i, j, 2) * mask[0][1] + topRight(i, j, 2) * mask[0][2] +
                        center(i, j, 2) * mask[1][1] + right(i, j, 2) * mask[1][2] +
                        bottom(i, j, 2) * mask[2][1] + bottomRight(i, j, 2) * mask[2][2];
                    setPixel(i, j, r / weight, g / weight, b / weight, fbuf);
                } else if (j == screenWidth - 1) {
                    float r, g, b;
                    int weight = mask[0][0] + mask[0][1] + mask[1][0] + mask[1][1] + mask[2][0] + mask[2][1];
                    r = topLeft(i, j, 0) * mask[0][0] + top(i, j, 0) * mask[0][1] +
                        left(i, j, 0) * mask[1][0] + center(i, j, 0) * mask[1][1] +
                        bottomLeft(i, j, 0) * mask[2][0] + bottom(i, j, 0) * mask[2][1];
                    g = topLeft(i, j, 1) * mask[0][0] + top(i, j, 1) * mask[0][1] +
                        left(i, j, 1) * mask[1][0] + center(i, j, 1) * mask[1][1] +
                        bottomLeft(i, j, 1) * mask[2][0] + bottom(i, j, 1) * mask[2][1];
                    b = topLeft(i, j, 2) * mask[0][0] + top(i, j, 2) * mask[0][1] +
                        left(i, j, 2) * mask[1][0] + center(i, j, 2) * mask[1][1] +
                        bottomLeft(i, j, 2) * mask[2][0] + bottom(i, j, 2) * mask[2][1];
                    setPixel(i, j, r / weight, g / weight, b / weight, fbuf);
                } else {
                    float r, g, b;
                    int weight = mask[0][0] + mask[0][1] + mask[0][2] + mask[1][0] + mask[1][1] + mask[1][2] + mask[2][0] + mask[2][1] + mask[2][2];
                    r = topLeft(i, j, 0) * mask[0][0] + top(i, j, 0) * mask[0][1] +
                        topRight(i, j, 0) * mask[0][2] + left(i, j, 0) * mask[1][0] +
                        center(i, j, 0) * mask[1][1] + right(i, j, 0) * mask[1][2] +
                        bottomLeft(i, j, 0) * mask[2][0] + bottom(i, j, 0) * mask[2][1] +
                        bottomRight(i, j, 0) * mask[2][2];
                    g = topLeft(i, j, 1) * mask[0][0] + top(i, j, 1) * mask[0][1] +
                        topRight(i, j, 1) * mask[0][2] + left(i, j, 1) * mask[1][0] +
                        center(i, j, 1) * mask[1][1] + right(i, j, 1) * mask[1][2] +
                        bottomLeft(i, j, 1) * mask[2][0] + bottom(i, j, 1) * mask[2][1] +
                        bottomRight(i, j, 1) * mask[2][2];
                    b = topLeft(i, j, 2) * mask[0][0] + top(i, j, 2) * mask[0][1] +
                        topRight(i, j, 2) * mask[0][2] + left(i, j, 2) * mask[1][0] +
                        center(i, j, 2) * mask[1][1] + right(i, j, 2) * mask[1][2] +
                        bottomLeft(i, j, 2) * mask[2][0] + bottom(i, j, 2) * mask[2][1] +
                        bottomRight(i, j, 2) * mask[2][2];
                    setPixel(i, j, r / weight, g / weight, b / weight, fbuf);
                }
            }
        }
    }
}

void keyCallback (GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_E:
                if (points.size() > 1 && !filled)
                    bresenham(points[points.size() - 1].first, points[points.size() - 1].second, points[0].first, points[0].second);
                break;
            case GLFW_KEY_C:
                clear();
                break;
            case GLFW_KEY_F:
                if (!filtered)
                    filter();
                filtered = !filtered;
                break;
            case GLFW_KEY_D:
                if (!filled && points.size() > 1)
                    fill();
                break;
        }
    }
}

void mouseButtonCallback (GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !filled)
    {
        std::pair<int, int> p;
        p.first = (int)xPos;
        p.second = (int)(screenHeight - yPos);
        points.push_back(p);
        if (points.size() > 1)
            bresenham(points[points.size() - 2].first, points[points.size() - 2].second, points[points.size() - 1].first, points[points.size() - 1].second);
    }
}

void windowsizeCallback (GLFWwindow* window, int width, int height)
{
    screenWidth = width;
    screenHeight = height;
    delete[] buf;
    delete[] fbuf;
    buf = new float[screenHeight * screenWidth * 3];
    fbuf = new float[screenHeight * screenWidth * 3];
    clear();
}
