// assignment1.cpp
// This program shows how the assignment is done.
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <Shlobj.h>

#define PI 3.1415926

#define IDM_CHANGE_COLOR 40001
#define IDM_CHANGE_POSITION 40002
#define IDM_CHANGE_DIRECTION 40003
#define IDM_SAVE 40004

TCHAR szBuffer[MAX_PATH] = {0};
bool savePic = false;

// Initial position and size
GLsizei facesize = 50;
GLsizei eyesize = 10;
GLfloat mouthMin = 1.7f * PI;
GLfloat mouthMax = 1.9f * PI;
GLfloat mouthStart = mouthMin;
GLfloat mouthDest = mouthMax;

// Step size
GLfloat step = 0.04f;

// Some menu-controled variables
bool eyeChange = false;
GLfloat colorOverlay = 1.0f;

// Keep track of windows changing width and height
GLfloat windowWidth;
GLfloat windowHeight;
int nScreenX, nScreenY;	// Screen Dimensions
int nPosX, nPosY;	    // Initial Screen Position

static int frameNum = 0;
unsigned char colorBuf[2560 * 1600 * 3];
char numStr[5];
char bmpFilename[50];

static LPCTSTR lpszAppName = "Assignment 3-2 - Gao Tong";

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

boolean SaveDIB24(const char* lpszFileName, DWORD dwWidth, DWORD dwHeight, void* lpvBits)
{
    HANDLE hFile = NULL;

    BOOL bOK;
    DWORD dwNumWritten;
    DWORD dwWidthAlign;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;

    __try {

        hFile = CreateFile(
            lpszFileName,
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            0,
            NULL
        );
        if (hFile == INVALID_HANDLE_VALUE) return 0;

        dwWidthAlign = ((dwWidth * sizeof(RGBTRIPLE) + 3) / 4) * 4;

        // BITMAPFILEHEADDER
        bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
        bmfh.bfType = ('B' | 'M' << 8);
        bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        bmfh.bfSize = bmfh.bfOffBits + dwWidthAlign*dwHeight;

        bOK = WriteFile(
            hFile,
            &bmfh,
            sizeof(bmfh),
            &dwNumWritten,
            NULL
        );
        if (!bOK) return 0;


        bmih.biSize = sizeof(BITMAPINFOHEADER);
        bmih.biWidth = dwWidth;
        bmih.biHeight = dwHeight;
        bmih.biPlanes = 1;
        bmih.biBitCount = 24;
        bmih.biCompression = 0;
        bmih.biSizeImage = 0;
        bmih.biXPelsPerMeter = 0;
        bmih.biYPelsPerMeter = 0;
        bmih.biClrUsed = 0;
        bmih.biClrImportant = 0;

        bOK = WriteFile(
            hFile,
            &bmih,
            sizeof(bmih),
            &dwNumWritten,
            NULL
        );
        if (!bOK) return 0;

        bOK = WriteFile(
            hFile,
            lpvBits,
            dwWidthAlign*dwHeight,
            &dwNumWritten,
            NULL
        );
        if (!bOK) return 0;

    }
    __finally {

        CloseHandle(hFile);

    }

    return 1;
}

// Called by timer routine to effect movement.
void IdleFunction(void)
{
    if (savePic) {
        //¶ÁÈ¡Ö¡»º´æ
        frameNum++;
        glReadPixels(0, 0, nScreenX, nScreenY, GL_BGR_EXT, GL_UNSIGNED_BYTE, colorBuf);

        sprintf_s(numStr, 10, "%04d", frameNum);
        numStr[4] = '\0';
        sprintf_s(bmpFilename, MAX_PATH, "%s%s%s.bmp", szBuffer, "/frame", numStr);
        SaveDIB24(bmpFilename, nScreenX, nScreenY, colorBuf);
    }

    if (mouthDest - mouthStart < step && step > 0)
        step = -step;
    if (mouthStart - mouthMin < -step && step < 0)
        step = -step;

    mouthStart += step;
}

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

// Called by AUX library to draw scene.
void RenderScene(void)
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

    wc.lpszMenuName = NULL;
    wc.lpszClassName = lpszAppName;

    // Register the window class
    if (RegisterClass(&wc) == 0)
        return FALSE;
    
    // Get the screen size
    //nScreenX = 800;
    //nScreenY = 600;
    nPosX = 0;
    nPosY = 0;

    // Get he Window handle and Device context to the desktop
    hDesktopWnd = GetDesktopWindow();
    hDesktopDC = GetDC(hDesktopWnd);

    // Get the screen size
    nScreenX = GetDeviceCaps(hDesktopDC, HORZRES);
    nScreenY = GetDeviceCaps(hDesktopDC, VERTRES);

    // Release the desktop device context
    ReleaseDC(hDesktopWnd, hDesktopDC);

    // Create the main application window
    hWnd = CreateWindow(lpszAppName, lpszAppName,

        // OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
        WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,

        // Window position and size
        nPosX, nPosY,
        nScreenX, nScreenY,

        NULL, NULL,
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

void getSavingPath() {
    BROWSEINFO bi;
    ZeroMemory(&bi, sizeof(BROWSEINFO));
    bi.hwndOwner = NULL;
    bi.pszDisplayName = szBuffer;
    bi.lpszTitle = TEXT ("Choose a path for saving screenshots:");
    bi.ulFlags = BIF_RETURNFSANCESTORS;
    LPITEMIDLIST idl = SHBrowseForFolder(&bi);
    if (NULL == idl)
    {
        return;
    }
    SHGetPathFromIDList(idl, szBuffer);
}

// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND hWnd, UINT	message, WPARAM	wParam, LPARAM lParam)
{
    static HGLRC hRC;		// Permenant Rendering context
    static HDC hDC;			// Private GDI Device context
    static HMENU hMenu, hMenuPopup;     // Windows Menu

    switch (message)
    {
    // Window creation, setup for OpenGL
    case WM_CREATE:
        // Create Menu
        hMenu = CreateMenu();
        hMenuPopup = CreateMenu();
        AppendMenu(hMenuPopup, MF_STRING, IDM_CHANGE_COLOR, "Change &Color");
        AppendMenu(hMenuPopup, MF_STRING, IDM_CHANGE_POSITION, "Change &Eye");
        AppendMenu(hMenuPopup, MF_STRING, IDM_CHANGE_DIRECTION, "Change &Speed");
        AppendMenu(hMenuPopup, MF_STRING, IDM_SAVE, "Save &Pics...");
        AppendMenu(hMenu, MF_POPUP, (LONG)hMenuPopup, "MyMenu");

        // Store the device context
        hDC = GetDC(hWnd);

        // Select the pixel format
        SetDCPixelFormat(hDC);

        // Create the rendering context and make it current
        hRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hRC);

        // Create a timer that fires 30 times a second
        SetTimer(hWnd, 1, 40, NULL);
        break;

    // Toggle Menu
    case WM_RBUTTONUP:
        TrackPopupMenu(hMenuPopup, TPM_RIGHTBUTTON, LOWORD(lParam), HIWORD(lParam), 0, hWnd, NULL);
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
            if (colorOverlay < 0.8f)
                colorOverlay = 1.0f;
            else
                colorOverlay = 0.5f;
            break;
        case IDM_CHANGE_POSITION:
            eyeChange = !eyeChange;
            break;
        case IDM_CHANGE_DIRECTION:
            if (fabs(step) < 0.03f)
                step *= 2;
            else
                step /= 2;
            break;
        case IDM_SAVE:
            getSavingPath();
            savePic = true;
            break;
        }
        break;

    default:   // Passes it on if unproccessed
        return (DefWindowProc(hWnd, message, wParam, lParam));

    }

    return (0L);
}
