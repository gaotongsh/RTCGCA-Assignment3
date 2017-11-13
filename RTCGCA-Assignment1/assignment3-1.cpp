/*
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/*
 *  assignment3-1.cpp
 *  This program is one assignment of RTCGCA course.
 */
#include <windows.h>
#include <GL/GL.h>
#include <glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415926

#define IDM_CHANGE_COLOR 40001
#define IDM_CHANGE_POSITION 40002
#define IDM_CHANGE_DIRECTION 40003

// Initial position and size
GLsizei facesize = 50;
GLsizei eyesize = 10;
GLfloat mouthMin = 1.7f * PI;
GLfloat mouthMax = 1.9f * PI;
GLfloat mouthStart = mouthMin;
GLfloat mouthDest = mouthMax;

// Step size
GLfloat step = 0.0005f;

// Some menu-controled variables
bool eyeChange = false;
GLfloat colorOverlay = 1.0f;

// Keep track of windows changing width and height
GLfloat windowWidth = 800;
GLfloat windowHeight = 600;

/*
* Function that handles the drawing of a circle using the triangle fan
* method. This will create a filled circle.
*/
void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius,
    GLfloat start = 0.0f, GLfloat dest = 2.0f * PI, GLfloat x0 = -1.0f, GLfloat y0 = -1.0f)
{
    int i;
    int triangleAmount = 50; //# of triangles used to draw circle

    if (x0 < 0.0f && y0 < 0.0f) {
        x0 = x;
        y0 = y;
    }

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x0, y0); // center of circle
    for (i = 0; i <= triangleAmount; i++) {
        glVertex2f(
            x + (radius * cos(start + i * (dest - start) / triangleAmount)),
            y + (radius * sin(start + i * (dest - start) / triangleAmount))
        );
    }
    glEnd();
}

void display(void)
{
    // Set background clearing color to sky blue.
    glClearColor(0.2f, 0.8f, 1.0f, 1.0f);
    if (colorOverlay < 0.9f)
        glClearColor(0.1f, 0.1f, 0.5f, 1.0f);

    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a grassland.
    glColor3ub(34 * colorOverlay, 139 * colorOverlay, 34 * colorOverlay);
    glRecti(0, 0, windowWidth + 2, 0.6 * windowHeight);

    // Draw a face.
    glColor3ub(252 * colorOverlay, 224 * colorOverlay, 203 * colorOverlay);
    drawFilledCircle(windowWidth / 2, windowHeight / 2, facesize);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawFilledCircle(windowWidth / 2 + 0.5 * facesize, windowHeight / 2 + 0.3 * facesize, eyesize);

    // If eyeChange, draw a different eye.
    if (eyeChange)
    {
        glColor3ub(252 * colorOverlay, 224 * colorOverlay, 203 * colorOverlay);
        drawFilledCircle(windowWidth / 2 + 0.5 * facesize, windowHeight / 2 + 0.1 * facesize, 1.2 * eyesize);
    }

    // Draw the mouse
    glColor3ub(255 * colorOverlay, 69 * colorOverlay, 0);
    drawFilledCircle(windowWidth / 2, windowHeight / 2, facesize,
        mouthStart, mouthDest, windowWidth / 2 + 0.3 * facesize, windowHeight / 2 - 0.3 * facesize);

    glutSwapBuffers();

    if (mouthDest - mouthStart < step && step > 0)
        step = -step;
    if (mouthStart - mouthMin < -step && step < 0)
        step = -step;

    mouthStart += step;
}

void reshape(int w, int h)
{
    // Prevent a divide by zero, when window is too short (you cant make a window of zero width).
    if (h == 0)
        h = 1;

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Reset the coordinate system before modifying
    glLoadIdentity();

    // Keep the square square, this time, save calculated width and height for later use
    if (w <= h)
    {
        windowHeight = 250.0f*h / w;
        windowWidth = 250.0f;
    }
    else
    {
        windowWidth = 250.0f*w / h;
        windowHeight = 250.0f;
    }

    // Set the clipping volume
    gluOrtho2D(0.0f, windowWidth, 0.0f, windowHeight);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
        exit(0);
        break;
    }
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Assignment 3-1 - Gao Tong");

	const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字
	const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台
	const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //返回当前OpenGL实现的版本号
	const GLubyte* gluVersion = gluGetString(GLU_VERSION); //返回当前GLU工具库版本

	printf("OpenGL实现厂商的名字：%s\n", name);
	printf("渲染器标识符：%s\n", biaoshifu);
	printf("OpenGL实现的版本号：%s\n", OpenGLVersion);
	printf("GLU工具库版本：%s\n", gluVersion);

	glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
	glutIdleFunc(display);
	glutMainLoop();
	return 0;
}
