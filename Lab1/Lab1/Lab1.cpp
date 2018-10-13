// Lab1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lab1.h"

#define MAX_LOADSTRING 100

// The RGB color in the source bitmap to treat as transparent
#define MASK 0xFFFFFF
#define STEP_SIZE 10

#define ELLIPSE_HEIGHT 90
#define ELLIPSE_WIDTH 90
#define ELLIPSE_COLOR 0x0099FFCC

#define RECTANGLE_HEIGHT 80
#define RECTANGLE_WIDTH 100
#define RECTANGLE_COLOR 0x009999FF

#define IDT_TIMER 1
#define TIMER_INTERVAL 2

enum FIGURE { ELLIPSE, RECTANGLE, PICTURE };
enum DIRECTION { UP, RIGHT, LEFT, DOWN, NONE };

typedef struct _OBJINFO
{
	// Type of object to be painted
	FIGURE figure;

	// Height and width of the painted object
	int width;
	int height;

	// Current coordinates of the cursor
	int x;
	int y;

	// Direction of movement of the object
	DIRECTION direction;
} OBJINFO;

// Global Variables:
HINSTANCE hInst;                                // Current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // The main window class name
HBITMAP hbmPicture = NULL;                      //todo comment
OBJINFO oObjInfo;                               //todo comment
BOOL bIsObjSelected = false;                    //todo comment

// The offset between coordinates of the upper-left corner of the object
// and coordinates of mouse at the moment of clicking on the object
int mouseOffsetX;
int mouseOffsetY;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
OBJINFO             InitializeObjInfo(FIGURE figure, DIRECTION direction, POINT position, int width, int height);
VOID                ChangeFigure(OBJINFO *obj, FIGURE figure, int width, int height);
VOID                PaintObject(HDC hdc, RECT* prc);
VOID                RecreateObject(HWND hWnd);

//VOID UpdatePosition(RECT wndRect, long objHeight, long objWidth);
VOID				MoveUp();
VOID				MoveDown(int bottomBorder, int objHeight);
VOID				MoveLeft();
VOID				MoveRight(int rightBorder, int objWidth);
VOID				MoveObjectOnArrowKey(HWND hWnd, WPARAM wParam, RECT wndRect);
BOOL				IsMouseOverObject(POINT mousePosition, OBJINFO obj);
VOID UpdatePosition(RECT rectWnd, POINT mousePos, OBJINFO *obj);

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

//todo comment
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
	RECT wndRect;  //todo comment

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

			oObjInfo = InitializeObjInfo(ELLIPSE, NONE, { 0, 0 }, ELLIPSE_WIDTH, ELLIPSE_HEIGHT);
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            // Parse the menu selections:
            switch (wmId)
            {
			case IDM_ELLIPSE:
				ChangeFigure(&oObjInfo, ELLIPSE, ELLIPSE_WIDTH, ELLIPSE_HEIGHT);
				RecreateObject(hWnd);  //todo comment
				break;
			case IDM_RECT:
				ChangeFigure(&oObjInfo, RECTANGLE, RECTANGLE_WIDTH, RECTANGLE_HEIGHT);
				RecreateObject(hWnd);  //todo comment
				break;
			case IDM_PICTURE:
				GetObject(hbmPicture, sizeof(bm), &bm);
				ChangeFigure(&oObjInfo, PICTURE, bm.bmWidth, bm.bmHeight);
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
	case WM_KEYDOWN: 
		{
			GetObject(hbmPicture, sizeof(BITMAP), &bm);
			GetClientRect(hWnd, &wndRect);
			MoveObjectOnArrowKey(hWnd, wParam, wndRect);
		}
		break;
	case WM_LBUTTONDOWN:
	{
		POINT ptMousePosition = { LOWORD(lParam), HIWORD(lParam) };
		GetObject(hbmPicture, sizeof(BITMAP), &bm);

		// User grab the object
		if (IsMouseOverObject(ptMousePosition, oObjInfo))
		{
			// The offset between coordinates of the upper-left corner of the object
			// and coordinates of the mouse at the moment of clicking on the object
			mouseOffsetX = ptMousePosition.x - oObjInfo.x;
			mouseOffsetY = ptMousePosition.y - oObjInfo.y;

			bIsObjSelected = true;
		}

		// The window will continue to receive WM_MOUSEMOVE messages
		// even if the mouse moves outside the window
		SetCapture(hWnd);
	}
	break;
	case WM_LBUTTONUP:
	{
		bIsObjSelected = false;

		// Stop to receive WM_MOUSEMOVE messages
		// if the mouse moves outside the window
		ReleaseCapture();
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (bIsObjSelected)
		{
			GetObject(hbmPicture, sizeof(BITMAP), &bm);
			GetClientRect(hWnd, &wndRect);

			// Current coordinates of the mouse taking into account the offset
			int x = GET_X_LPARAM(lParam) - mouseOffsetX;
			int y = GET_Y_LPARAM(lParam) - mouseOffsetY;

			UpdatePosition(wndRect, { x,y }, &oObjInfo);
			RecreateObject(hWnd);
		}
	}
	break;
	case WM_MOUSEWHEEL:
	{
		GetObject(hbmPicture, sizeof(BITMAP), &bm);
		GetClientRect(hWnd, &wndRect);

		// Indicates which keys are down
		int keys = GET_KEYSTATE_WPARAM(wParam);

		// Positive value indicates that the wheel was rotated 
		// away from the user, negative - toward the user
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);

		if (keys == MK_SHIFT)
		{
			if (delta > 0)
				MoveRight(wndRect.right, oObjInfo.width);
			else if (delta < 0)
				MoveLeft();
		}
		else
		{
			if (delta > 0)
				MoveUp();
			else if (delta < 0)
				MoveDown(wndRect.bottom, oObjInfo.height);
		}

		RecreateObject(hWnd);
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

// Return the OBJINFO structure with initialized fields
OBJINFO InitializeObjInfo(FIGURE figure, DIRECTION direction, POINT position, int width, int height)
{
	OBJINFO obj;

	obj.figure = figure;
	obj.direction = direction;
	obj.x = position.x;
	obj.y = position.y;
	obj.height = height;
	obj.width = width;

	return obj;
}

// Change info about object in OBJINFO structure
VOID ChangeFigure(OBJINFO *obj, FIGURE figure, int width, int height)
{
	(*obj).figure = figure;
	(*obj).height = height;
	(*obj).width = width;
}

//todo comment
VOID RecreateObject(HWND hWnd)
{
	// Set the whole window to be redrawn when the next WM_PAINT message occurs
	InvalidateRect(hWnd, NULL, TRUE);
}

//todo comment
VOID PaintObject(HDC hdc, RECT* prc)
{
	HBRUSH hBrush, hOldBrush;

	switch (oObjInfo.figure)
	{
		case ELLIPSE:
			hBrush = CreateSolidBrush(ELLIPSE_COLOR);

			// Return an old brush handler and replace it with the new one
			hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			
			// Draw ellipse
			Ellipse(hdc, oObjInfo.x, oObjInfo.y, oObjInfo.x + ELLIPSE_WIDTH, oObjInfo.y + ELLIPSE_HEIGHT);
			
			// Return the previous brush
			SelectObject(hdc, hOldBrush);
			DeleteObject(hBrush);

			break;

		case RECTANGLE:
			hBrush = CreateSolidBrush(RECTANGLE_COLOR);

			// Return an old brush handler and replace it with the new one
			hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			// Draw rectangle
			Rectangle(hdc, oObjInfo.x, oObjInfo.y, oObjInfo.x + RECTANGLE_WIDTH, oObjInfo.y + RECTANGLE_HEIGHT);

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
			TransparentBlt(hdc, oObjInfo.x, oObjInfo.y, bmp.bmWidth, bmp.bmHeight,
				hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, MASK);

			SelectObject(hMemDC, hPrevBmp);
			DeleteDC(hMemDC);
			DeleteObject(hPrevBmp);

			break;
	}
}

//todo comment
//VOID UpdatePosition(RECT wndRect, long objHeight, long objWidth)
//{
//	switch (dObjDirection)
//	{
//		case UP:
//		{
//			MoveUp();
//			break;
//		}
//		case LEFT:
//		{
//			MoveLeft();
//			break;
//		}
//		case DOWN:
//		{
//			MoveDown(wndRect.bottom, objHeight);
//			break;
//		}
//		case RIGHT:
//		{
//			MoveRight(wndRect.right, objWidth);
//			break;
//		}
//	}
//}

//todo commnet
BOOL IsMouseOverObject(POINT mousePosition, OBJINFO obj)
{
	return ((mousePosition.x < obj.x + obj.width) && (mousePosition.x > obj.x)
		&& (mousePosition.y < obj.y + obj.height) && (mousePosition.y > obj.y));
}

// Update position of the object on mouse move
VOID UpdatePosition(RECT rectWnd, POINT mousePos, OBJINFO *obj)
{
	if (mousePos.x < 0)
		(*obj).x = 0;
	else if (mousePos.x + (*obj).width > rectWnd.right)
		(*obj).x = rectWnd.right - (*obj).width;
	else
		(*obj).x = mousePos.x;

	if (mousePos.y < 0)
		(*obj).y = 0;
	else if (mousePos.y + (*obj).height > rectWnd.bottom)
		(*obj).y = rectWnd.bottom - (*obj).height;
	else
		(*obj).y = mousePos.y;
}

VOID MoveUp()
{
	int posLimit = 0;  	// the topmost position
	int nextPosition = oObjInfo.y - STEP_SIZE;

	if (nextPosition < posLimit)
	{
		oObjInfo.y = posLimit;
		oObjInfo.direction = DOWN;   // rebound
	}
	else
	{
		oObjInfo.y = nextPosition;
		oObjInfo.direction = UP;
	}
}

VOID MoveDown(int bottomBorder, int objHeight)
{
	int posLimit = bottomBorder - objHeight;  	// the lowest position
	int nextPosition = oObjInfo.y + STEP_SIZE;

	if (nextPosition > posLimit)
	{
		oObjInfo.y = posLimit;
		oObjInfo.direction = UP;   // rebound
	}
	else
	{
		oObjInfo.y = nextPosition;
		oObjInfo.direction = DOWN;
	}
}

VOID MoveLeft()
{
	int posLimit = 0;  	// the leftmost position
	int nextPosition = oObjInfo.x - STEP_SIZE;

	if (nextPosition < posLimit)
	{
		oObjInfo.x = posLimit;
		oObjInfo.direction = RIGHT;   // rebound
	}
	else
	{
		oObjInfo.x = nextPosition;
		oObjInfo.direction = LEFT;
	}
}

VOID MoveRight(int rightBorder, int objWidth)
{
	int posLimit = rightBorder - objWidth;  	// the rightmost position
	int nextPosition = oObjInfo.x + STEP_SIZE;

	if (nextPosition > posLimit)
	{
		oObjInfo.x = posLimit;
		oObjInfo.direction = LEFT;   // rebound
	}
	else
	{
		oObjInfo.x = nextPosition;
		oObjInfo.direction = RIGHT;
	}
}

VOID MoveObjectOnArrowKey(HWND hWnd, WPARAM wParam, RECT wndRect)
{
	int wmId = LOWORD(wParam);
	switch (wmId)
	{
		case VK_DOWN:
			MoveDown(wndRect.bottom, oObjInfo.height);
			break;
		case VK_UP:
			MoveUp();
			break;
		case VK_LEFT:		
			MoveLeft();
			break;
		case VK_RIGHT:
			MoveRight(wndRect.right, oObjInfo.width);
			break;
	}

	//todo comment
	InvalidateRect(hWnd, NULL, TRUE);
}