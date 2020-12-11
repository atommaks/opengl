#include <iostream>
#include <GLFW/glfw3.h>

enum class Figure {SQUARE, TRIANGLE};
enum class Color {RED, BLUE};

Figure figure = Figure::TRIANGLE;
Color color = Color::RED;
const GLint WIDTH = 800, HEIGHT = 600;

using namespace std;

void changeColor () {
    if (color == Color::RED)
        glColor3d(1, 0, 0);
    else
        glColor3d(0, 0, 1);
}

void drawTriangle () {
    glLineWidth(2);
    glBegin(GL_TRIANGLES);
    glVertex2f(100,50);
    glVertex2f(0,100);
    glVertex2f(0,0);
    glEnd();
}

void drawSquare () {
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 100.0f);   // top left
    glVertex2f(100.0f, 100.0f); // top right
    glVertex2f(100.0f, 0.0f);   // bottom right
    glVertex2f(0.0f, 0.0f);     // bottom left
    glEnd();
}

static void mouse_button_callback (GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS)
            color = Color::BLUE;
        if (action == GLFW_RELEASE)
            color = Color::RED;
    }
}

static void key_callback (GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        figure = Figure::SQUARE;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        figure = Figure::TRIANGLE;
}

int main () {
    GLFWwindow* window;
    if (!glfwInit())
        return -1;
    
    window = glfwCreateWindow(WIDTH, HEIGHT, "Lab01", NULL, NULL);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    if (NULL == window) {
        cerr << "Failed to create GLFW-window" << endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    while (!glfwWindowShouldClose(window)) {
        glViewport(0, 0, screenWidth, screenHeight);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.f, WIDTH, HEIGHT, 0.f, 0.f, 1.f);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        changeColor();
        if (figure == Figure::TRIANGLE)
            drawTriangle();
        else
            drawSquare();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
