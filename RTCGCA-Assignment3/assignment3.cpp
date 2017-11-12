// assignment1.cpp
// This program shows ...
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <math.h>

#define PI 3.1415926

#define IDM_CHANGE_COLOR 40001
#define IDM_CHANGE_POSITION 40002
#define IDM_CHANGE_DIRECTION 40003

// Initial square position and size
GLfloat x11 = 100.0f;
GLfloat y11 = 150.0f;
GLsizei rsize = 50;
GLsizei facesize = 50;
GLsizei eyesize = 10;

// Step size in x and y directions
// (number of pixels to move each time)
GLfloat xstep = 1.0f;
GLfloat ystep = 1.0f;

// Keep track of windows changing width and height
GLfloat windowWidth;
GLfloat windowHeight;

static LPCTSTR lpszAppName = "Assignment - Gao Tong";

// Declaration for Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Set Pixel Format function - forward declaration
void SetDCPixelFormat(HDC hDC);

void ChangeSize(GLsizei w, GLsizei h)
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


// Called by timer routine to effect movement.
void IdleFunction(void)
{
    // Reverse direction when you reach left or right edge
    if (x11 > windowWidth - rsize || x11 < 0)
        xstep = -xstep;

    // Reverse direction when you reach top or bottom edge
    if (y11 > windowHeight - rsize || y11 < 0)
        ystep = -ystep;


    // Check bounds. This is incase the window is made smaller and the rectangle is outside the new clipping volume
    if (x11 > windowWidth - rsize)
        x11 = windowWidth - rsize - 1;

    if (y11 > windowHeight - rsize)
        y11 = windowHeight - rsize - 1;

    // Actually move the square
    x11 += xstep;
    y11 += ystep;
}

/*
 * Function that handles the drawing of a circle using the triangle fan
 * method. This will create a filled circle.
 */
void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius) {
    int i;
    int triangleAmount = 50; //# of triangles used to draw circle

                             //GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * PI;

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // center of circle
        for (i = 0; i <= triangleAmount; i++) {
            glVertex2f(
                x + (radius * cos(i * twicePi / triangleAmount)),
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();
}

// Called by AUX library to draw scene.
void RenderScene(void)
{
    // Set background clearing color to sky blue.
    glClearColor(0.2f, 0.8f, 1.0f, 1.0f);

    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a grassland.
    glColor3ub(34, 139, 34);
    glRecti(0, 0, windowWidth + 2, 0.6 * windowHeight);

    // Draw a face.
    glColor3ub(252, 224, 203);
    drawFilledCircle(windowWidth / 2, windowHeight / 2, facesize);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawFilledCircle(windowWidth / 2 + 0.5 * facesize, windowHeight / 2 + 0.3 * facesize, eyesize);
}


// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
{
    int nPixelFormat;

    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
        1,								// Version of this structure	
        PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
        PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
        PFD_DOUBLEBUFFER,				// Double buffered mode
        PFD_TYPE_RGBA,					// RGBA Color mode
        32,								// Want 32 bit color 
        0,0,0,0,0,0,					// Not used to select mode
        0,0,							// Not used to select mode
        0,0,0,0,0,						// Not used to select mode
        16,								// Size of depth buffer
        0,								// Not used to select mode
        0,								// Not used to select mode
        0,	            				// Not used to select mode
        0,								// Not used to select mode
        0,0,0};							// Not used to select mode

                                        // Choose a pixel format that best matches that described in pfd
    nPixelFormat = ChoosePixelFormat(hDC, &pfd);

    // Set the pixel format for the device context
    SetPixelFormat(hDC, nPixelFormat, &pfd);
}


// Entry point of all Windows programs
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG			msg;				// Windows message structure
    WNDCLASS	wc;		            // Windows class structure
    HWND		hWnd;			    // Storeage for window handle
    HWND		hDesktopWnd;		// Storeage for desktop window handle
    HDC			hDesktopDC; 		// Storeage for desktop window device context
    HMENU       hMenu;              // Windows Menu
    int			nScreenX, nScreenY;	// Screen Dimensions
    int			nPosX, nPosY;	    // Initial Screen Position

                                    // Register Window style
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    // No need for background brush for OpenGL window
    wc.hbrBackground = NULL;

    wc.lpszMenuName = lpszAppName;
    wc.lpszClassName = lpszAppName;

    // Register the window class
    if (RegisterClass(&wc) == 0)
        return FALSE;
    
    // Get the screen size
    nScreenX = 800;
    nScreenY = 600;
    nPosX = 100;
    nPosY = 100;

    // Create Menu
    hMenu = CreateMenu();
    AppendMenu(hMenu, MF_STRING, IDM_CHANGE_COLOR, "Change &Color");
    AppendMenu(hMenu, MF_STRING, IDM_CHANGE_POSITION, "Change &Position");
    AppendMenu(hMenu, MF_STRING, IDM_CHANGE_DIRECTION, "Change &Direction");

    // Create the main application window
    hWnd = CreateWindow(lpszAppName, lpszAppName,

        // OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,

        // Window position and size
        nPosX, nPosY,
        nScreenX, nScreenY,

        NULL,
        hMenu,
        hInstance,
        NULL);


    // If window was not created, quit
    if (hWnd == NULL)
        return FALSE;


    // Display the window
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    // Process application messages until the application closes
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}



// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND hWnd, UINT	message, WPARAM	wParam, LPARAM lParam)
{
    static HGLRC hRC;		// Permenant Rendering context
    static HDC hDC;			// Private GDI Device context

    switch (message)
    {
    // Window creation, setup for OpenGL
    case WM_CREATE:
        // Store the device context
        hDC = GetDC(hWnd);

        // Select the pixel format
        SetDCPixelFormat(hDC);

        // Create the rendering context and make it current
        hRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hRC);

        // Create a timer that fires 30 times a second
        SetTimer(hWnd, 33, 1, NULL);
        break;

    // Window is being destroyed, cleanup
    case WM_DESTROY:
        // Kill the timer that we created
        KillTimer(hWnd, 101);

        // Deselect the current rendering context and delete it
        wglMakeCurrent(hDC, NULL);
        wglDeleteContext(hRC);

        // Tell the application to terminate after the window is gone.
        PostQuitMessage(0);
        break;

    // Window is resized.
    case WM_SIZE:
        // Call our function which modifies the clipping
        // volume and viewport
        ChangeSize(LOWORD(lParam), HIWORD(lParam));
        break;

    // Timer, moves and bounces the rectangle, simply calls  our previous OnIdle function, then invalidates the 
    // window so it will be redrawn.
    case WM_TIMER:
        IdleFunction();
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    // handle keyboard events for input
    case WM_CHAR:

        switch (wParam)
        {
        case '\x1B':	// Esc Key
        case 'q':
        case 'Q':
            DestroyWindow(hWnd);
            break;
        }
        break;

    // The painting function.  This message sent by Windows whenever the screen needs updating.
    case WM_PAINT:
        // Call OpenGL drawing code
        RenderScene();

        // Call function to swap the buffers
        SwapBuffers(hDC);

        // Validate the newly painted client area
        ValidateRect(hWnd, NULL);
        break;

    // The Menu Command
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_CHANGE_COLOR:
        case IDM_CHANGE_POSITION:
        case IDM_CHANGE_DIRECTION:
            break;
        }
        break;

    default:   // Passes it on if unproccessed
        return (DefWindowProc(hWnd, message, wParam, lParam));

    }

    return (0L);
}
