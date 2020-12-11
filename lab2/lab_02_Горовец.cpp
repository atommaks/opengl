#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

void keyCallback (GLFWwindow *window, int key, int scancode, int action, int mods);
void drawCube ();

bool isColored = false;
GLfloat translationX = 0.f, translationY = 0.f, translationZ = -0.4f;
GLfloat rotationX = 0.f, rotationY = 0.f, rotationZ = 0.f;
GLfloat size = 1.f;

int main (int argc, char** argv)
{
    GLFWwindow *window;
    
    if (!glfwInit())
        return -1;
    
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lab2", nullptr, nullptr);
    
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);
    
    while (!glfwWindowShouldClose(window))
    {
        glViewport(0.f, 0.f, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        
        glMatrixMode(GL_PROJECTION);
        float a = asin(tanf(M_PI / 6.f)), b = M_PI_4, ratio = (float) screenWidth / screenHeight;
        float m[16] =
        {
            cosf(b) / ratio, sinf(b) * sinf(a), 0.f, 0.f,
            0.f, cosf(a), 0.f, 0.f,
            sinf(b) / ratio, -sinf(a) * cosf(b), 0.f, 0.f,
            0.f, 0.f, 0.f, 1.f
        };
        glLoadMatrixf(m);
        
        glMatrixMode(GL_MODELVIEW);
        //dynamic cube
        glPushMatrix();
        glTranslatef(translationX, translationY, translationZ);
        glRotatef(rotationX, 1.f, 0.f, 0.f);
        glRotatef(rotationY, 0.f, 1.f, 0.f);
        glRotatef(rotationZ, 0.f, 0.f, 1.f);
        glScaled(size, size, size);
        glScaled(0.5f, 0.5f, 0.5f);
        drawCube();
        glPopMatrix();
        
        //static cube
        glPushMatrix();
        glTranslatef(-0.75f, -0.5f, -0.5f);
        glScaled(0.25f, 0.25f, 0.25f);
        drawCube();
        glPopMatrix();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}


void drawCube ()
{
    #define LEFT_BOTTOM_NEAR    -0.5f, -0.5f, +0.5f
    #define RIGHT_BOTTOM_NEAR   +0.5f, -0.5f, +0.5f
    #define RIGHT_TOP_NEAR      +0.5f, +0.5f, +0.5f
    #define LEFT_TOP_NEAR       -0.5f, +0.5f, +0.5f
    #define LEFT_BOTTOM_FAR     -0.5f, -0.5f, -0.5f
    #define RIGHT_BOTTOM_FAR    +0.5f, -0.5f, -0.5f
    #define RIGHT_TOP_FAR       +0.5f, +0.5f, -0.5f
    #define LEFT_TOP_FAR        -0.5f, +0.5f, -0.5f
    
    GLfloat vertices [] =
    {
        //back face
        LEFT_BOTTOM_FAR,
        LEFT_TOP_FAR,
        RIGHT_TOP_FAR,
        RIGHT_BOTTOM_FAR,
        //front face
        LEFT_BOTTOM_NEAR,
        RIGHT_BOTTOM_NEAR,
        RIGHT_TOP_NEAR,
        LEFT_TOP_NEAR,
        //right face
        RIGHT_BOTTOM_FAR,
        RIGHT_TOP_FAR,
        RIGHT_TOP_NEAR,
        RIGHT_BOTTOM_NEAR,
        //left face
        LEFT_BOTTOM_FAR,
        LEFT_BOTTOM_NEAR,
        LEFT_TOP_NEAR,
        LEFT_TOP_FAR,
        //top face
        RIGHT_TOP_FAR,
        LEFT_TOP_FAR,
        LEFT_TOP_NEAR,
        RIGHT_TOP_NEAR,
        //bottom face
        RIGHT_BOTTOM_FAR,
        RIGHT_BOTTOM_NEAR,
        LEFT_BOTTOM_NEAR,
        LEFT_BOTTOM_FAR
    };
    
    GLubyte colors[] =
    {
        255, 0, 0,
        255, 20, 147,
        255, 69, 0,
        0, 255, 255,
        255, 0, 255,
        0, 0, 255,
    };
    
    GLenum mode = GL_LINE_LOOP;
    if (isColored)
        mode = GL_QUADS;
    
    for (int i = 0; i < 6; i++)
    {
        glBegin(mode);
        if (isColored)
            glColor3ub(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]);
        else
            glColor3ub(255, 255, 255);
        glVertex3f(vertices[i * 12], vertices[i * 12 + 1], vertices[i * 12 + 2]);
        glVertex3f(vertices[i * 12 + 3], vertices[i * 12 + 4], vertices[i * 12 + 5]);
        glVertex3f(vertices[i * 12 + 6], vertices[i * 12 + 7], vertices[i * 12 + 8]);
        glVertex3f(vertices[i * 12 + 9], vertices[i * 12 + 10], vertices[i * 12 + 11]);
        glEnd();
    }
}

void keyCallback (GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        const GLfloat translationSpeed = 0.1f, rotationSpeed = 2.f, sizeSpeed = 0.25f;
        switch (key)
        {
            case GLFW_KEY_W:
                translationX += translationSpeed;
                break;
            case GLFW_KEY_S:
                translationX -= translationSpeed;
                break;
            case GLFW_KEY_A:
                translationZ -= translationSpeed;
                break;
            case GLFW_KEY_D:
                translationZ += translationSpeed;
                break;
            case GLFW_KEY_E:
                translationY += translationSpeed;
                break;
            case GLFW_KEY_Q:
                translationY -= translationSpeed;
                break;
            case GLFW_KEY_LEFT:
                rotationX -= rotationSpeed;
                break;
            case GLFW_KEY_RIGHT:
                rotationX += rotationSpeed;
                break;
            case GLFW_KEY_UP:
                rotationZ += rotationSpeed;
                break;
            case GLFW_KEY_DOWN:
                rotationZ -= rotationSpeed;
                break;
            case GLFW_KEY_O:
                rotationY += rotationSpeed;
                break;
            case GLFW_KEY_P:
                rotationY -= rotationSpeed;
                break;
            case GLFW_KEY_EQUAL:
                size += sizeSpeed;
                break;
            case GLFW_KEY_MINUS:
                if (size != 0.25f)
                    size -= sizeSpeed;
                break;
            case GLFW_KEY_C:
                isColored = !isColored;
                break;
        }
    }
}
