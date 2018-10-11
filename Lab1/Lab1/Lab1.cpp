// Lab1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lab1.h"

#define MAX_LOADSTRING 100

// The RGB color in the source bitmap to treat as transparent
#define MASK 0xFFFFFF

#define ELLIPSE_HEIGHT 90
#define ELLIPSE_WIDTH 90
#define ELLIPSE_COLOR 0x0099FFCC

#define RECTANGLE_HEIGHT 80
#define RECTANGLE_WIDTH 100
#define RECTANGLE_COLOR 0x009999FF

enum Figure { ELLIPSE, RECTANGLE, PICTURE };

// Global Variables:
HINSTANCE hInst;                                // Current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // The main window class name

HBITMAP hbmPicture = NULL;       //todo comment
Figure figure = ELLIPSE;        // Type of object to be painted
POINT ptCurPos = { 0, 0 };      // Current coordinates of the cursor

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
VOID PaintObject(HDC hdc, RECT* prc);
VOID RecreateObject(HWND hWnd);

// The application entry point
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,          // The current instance of tha application
                     _In_opt_ HINSTANCE hPrevInstance,   // Always null
                     _In_ LPWSTR    lpCmdLine,           // The command line for tha application
                     _In_ int       nCmdShow)            // Controls how the window is to be shown.
{
	// Macros to suppress the warnings about unreferenced parameters
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);

	// Register the main window class
    MyRegisterClass(hInstance);

    // Perform application initialization
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	// Load an accelerator table
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex; // Special structure for the window class registration

    wcex.cbSize = sizeof(WNDCLASSEX); // Size of structure used for serialization (?)

    wcex.style          = CS_HREDRAW | CS_VREDRAW; // Redraw the window if the height or the width were changed
    wcex.lpfnWndProc    = WndProc;  // Procedure used as a message listener
    wcex.cbClsExtra     = 0;  // Additional flag values
    wcex.cbWndExtra     = 0;  // Additional flag values
    wcex.hInstance      = hInstance;  // Current application instance
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB1); // The Menu name
    wcex.lpszClassName  = szWindowClass;  // This window's class name
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	// Return an unique identifier for the window
    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   // Initialize a handle to the main window
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_CREATE   - 
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BITMAP bm; //todo comment

    switch (message)
    {
	case WM_CREATE:
		{
			// Load the specified bitmap descriptor
			hbmPicture = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PICTURE)); 
			if (hbmPicture == NULL)
			{
				MessageBox(hWnd, (LPCWSTR)L"Couldn't load the specified bitmap.", (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
			}
			else
			{
				// Load the bitmap 
				// todo why??
				GetObject(hbmPicture, sizeof(bm), &bm);
			}
			break;
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDM_ELLIPSE:
				figure = ELLIPSE;
				RecreateObject(hWnd);  //todo comment
				break;
			case IDM_RECT:
				figure = RECTANGLE;
				RecreateObject(hWnd);  //todo comment
				break;
			case IDM_PICTURE:
				figure = PICTURE;
				RecreateObject(hWnd);
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
			// The window's coordinates
			RECT rectWindow;

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

			// Retrieve the coordinates of the window's client area
			GetClientRect(hWnd, &rectWindow);
			// Redraw the specified area
			PaintObject(hdc, &rectWindow);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		// Causes the message loop to end
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//todo comment
VOID RecreateObject(HWND hWnd)
{
	//todo objDirection = DIRECTION_NONE;
	// Set the whole window to be redrawn when the next WM_PAINT message occurs
	InvalidateRect(hWnd, NULL, TRUE);
}

//todo comment
VOID PaintObject(HDC hdc, RECT* prc)
{
	HBRUSH hBrush, hOldBrush;

	switch (figure)
	{
		case ELLIPSE:
			hBrush = CreateSolidBrush(ELLIPSE_COLOR);

			// Return an old brush handler and replace it with the new one
			hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			
			// Draw ellipse
			Ellipse(hdc, ptCurPos.x, ptCurPos.y, ptCurPos.x + ELLIPSE_WIDTH, ptCurPos.y + ELLIPSE_HEIGHT);
			
			// Return the previous brush
			SelectObject(hdc, hOldBrush);
			DeleteObject(hBrush);

			break;

		case RECTANGLE:
			hBrush = CreateSolidBrush(RECTANGLE_COLOR);

			// Return an old brush handler and replace it with the new one
			hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			// Draw rectangle
			Rectangle(hdc, ptCurPos.x, ptCurPos.y, ptCurPos.x + RECTANGLE_WIDTH, ptCurPos.y + RECTANGLE_HEIGHT);

			// Return the previous brush
			SelectObject(hdc, hOldBrush);
			DeleteObject(hBrush);

			break;

		case PICTURE:
			BITMAP bmp;
			//todo whyyy
			GetObject(hbmPicture, sizeof(BITMAP), &bmp);

			//todo comment
			HDC hMemDC = CreateCompatibleDC(hdc);
			//todo comment
			HBITMAP hPrevBmp = (HBITMAP)SelectObject(hMemDC, hbmPicture);
			// Transfer the picure to the device context 
			TransparentBlt(hdc, ptCurPos.x, ptCurPos.y, bmp.bmWidth, bmp.bmHeight, 
				hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, MASK);

			SelectObject(hMemDC, hPrevBmp);
			DeleteDC(hMemDC);
			DeleteObject(hPrevBmp);

			break;
	}
}
