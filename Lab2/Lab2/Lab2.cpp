// Lab2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lab2.h"

#define MAX_LOADSTRING 100

#define PEN_STYLE PS_SOLID
#define PEN_WIDTH 2
#define PEN_COLOR 0x000000

#define CHAR_WIDTH 10
#define CHAR_HEIGHT 20
#define INDENT 5

typedef std::vector<std::string> STRINGVECTOR;
typedef std::vector<STRINGVECTOR> STRINGTABLE;

typedef struct _TABLE
{
	INT numOfColums = 0; // Number of colums in the table
	INT numOfRows = 0;   // Number of rows in the table
	STRINGTABLE text;    // "Two-dimensional array" of strings to be dispalayed as a table 
} TABLE;

// Global Variables:
HINSTANCE hInst;                                // Current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // The main window class name
WCHAR szFileName[MAX_LOADSTRING];               // Name of the file with strings to be showed in the table
TABLE table;                                    // Struct that contains all info about table

INT scrolledWidth = 0;      // Width of the scrolled part of the table
INT tableBottomY = 0;       // Y coordinate of the table's bottom border

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, INT);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Edit(HWND, UINT, WPARAM, LPARAM);
VOID			    GetUserFileName(HWND, WCHAR []);
BOOL				LoadTextFromFile(WCHAR szFileName[], STRINGTABLE *table, INT numOfColumns);
VOID				DrawTable(HWND hWnd, HDC hdc, TABLE table);
VOID				WriteText(HDC hdc, RECT clientRect, TABLE table, INT columnWidth);
VOID				WriteRow(HDC hdc, RECT clientRect, TABLE table, INT rowIndex, INT columnWidth);
VOID				RefreshWindow(HWND hWnd);
INT					GetNumOfCharsToWrite(HDC hdc, INT columnWidth, std::wstring str);
BOOL				IsScrolling(HWND hWnd, WPARAM wParam);
VOID				UpdateTable(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));

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
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
//  WM_COMMAND    - process the application menu
//  WM_PAINT      - Paint the main window
//  WM_DESTROY    - post a quit message and return
//	WM_SIZE       - redraw table if it's necessary when size of the window changes
//  WM_MOUSEWHELL - redraw table on scrolling 
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hWndEdit = NULL;
	HWND hWndButton;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDM_OPEN:
				{
					// Show open file dialog and return the file name 
					GetUserFileName(hWnd, szFileName);
					// Show the edit dialog for entering the number of columns in the table
					if (szFileName[0] != '\0' && DialogBox(hInst, MAKEINTRESOURCE(IDD_EDITBOX), hWnd, Edit))
					{
						// Load lines from the file to the table.text
						table.numOfRows = LoadTextFromFile(szFileName, &(table.text), table.numOfColums);
						RefreshWindow(hWnd);
					}
				}
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
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			if (table.numOfColums != 0)
				DrawTable(hWnd, hdc, table);
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			scrolledWidth = 0;
			UpdateTable(hWnd);
		}
		break;
	case WM_MOUSEWHEEL:
		if (IsScrolling(hWnd, wParam))
		{
			UpdateTable(hWnd);
		}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box
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

// Message handler for edit box
INT_PTR CALLBACK Edit(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Entered string
	WCHAR lpstrColums[3];   
	// Number of entered characters
	WORD cchColums;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			// Get number of characters
			cchColums = (WORD)SendDlgItemMessage(hDlg, IDEDIT, EM_LINELENGTH, (WPARAM)0, (LPARAM)0);
			
			//  Check number of entered characters
			if (cchColums >= 2)
			{
				MessageBox(hDlg, L"Too many colums. Try again.", L"Error",	MB_OK);
				return (INT_PTR)FALSE;
			}
			else if (cchColums == 0)
			{
				MessageBox(hDlg, L"No characters entered. Try again.",	L"Error", MB_OK);
				return (INT_PTR)FALSE;
			}

			// Put the number of characters into first word of buffer 
			*((LPWORD)lpstrColums) = cchColums;

			// Get the characters. The size in the first word of the lpstrColumns is overwritten by the copied line
			SendDlgItemMessage(hDlg, IDEDIT, EM_GETLINE, (WPARAM)0,	(LPARAM)lpstrColums);

			// Null-terminate the string 
			lpstrColums[cchColums] = 0;
			
			// Convert string with number of colums to int value
			table.numOfColums = _wtoi(lpstrColums);

			// Check the conversion result
			if (table.numOfColums == 0)
			{
				MessageBox(hDlg, L"Invalid input. Try again.", L"Error", MB_OK);
				return (INT_PTR)FALSE;
			}

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Show the open file dialog box and put the file name to the szFileName parameter 
VOID GetUserFileName(HWND hWnd, WCHAR szFileName[])
{
	// Structure that contains information used to display the open file dialog box
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	WCHAR szFile[MAX_LOADSTRING];

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile; // name of the selected file
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile); // max lenght of the file name
	ofn.lpstrFilter = L"Text\0*.txt\0"; // only text files
	ofn.nFilterIndex = 1;  // text files
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Show the open file dialog box
	GetOpenFileName(&ofn);
	// Copy the file name to the szFileName
	wcscpy_s(szFileName, wcslen(szFile) + 1, szFile);
}

// Read the text file and put it's lines to the STRINGTABLE
// Return number of rows in the table
INT LoadTextFromFile(WCHAR szFileName[], STRINGTABLE *table, INT numOfColumns)
{
	// Convert wchar_t* to std::string
	std::wstring ws(szFileName);
	std::string fileName(ws.begin(), ws.end());

	// Create internal file stream
	std::ifstream fin(fileName.c_str());
	if (!fin)
		return false;

	std::string str;
	STRINGVECTOR row;
	INT rowsCount = 0;     

	// Read the file line by line
	while (!fin.eof())
	{
		for (int i = 0; i < numOfColumns; i++)
		{
			if (fin.eof())
			{
				// Incomplete row is supplemented with empty strings
				str = "";
			}
			else
			{
				// Read the next line
				std::getline(fin, str);
			}
			// Add it to the row
			row.push_back(str);
		}

		// Add row to the table text
		(*table).push_back(row);
		rowsCount++;
		row.clear();
	}

	fin.close();
	return rowsCount;
}

// Set the window to be redrawn
VOID RefreshWindow(HWND hWnd)
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	InvalidateRect(hWnd, &clientRect, TRUE);
}

VOID DrawTable(HWND hWnd, HDC hdc, TABLE table)
{
	// Set a new pen for drawing table lines
	HPEN pen = CreatePen(PEN_STYLE, PEN_WIDTH, PEN_COLOR);
	HGDIOBJ oldPen = SelectObject(hdc, pen);

	// Set a new font for text in the table
	HFONT font = CreateFont(CHAR_HEIGHT, CHAR_WIDTH, 0, 0, FW_NORMAL, false, false, false, 0, 0, 0, 0, 0, NULL);
	HGDIOBJ oldFont = SelectObject(hdc, font);

	// Get coordinates of the client area
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// Calculate width of columns of the table
	int columnWidth = clientRect.right / table.numOfColums;
	
	// Current x coordinate
	int x = 0;

	// Write text and draw horisontal lines
	WriteText(hdc, clientRect, table, columnWidth);

	// Draw vertical lines
	for (int i = 0; i < table.numOfColums - 1; i++)
	{
		x += columnWidth;
		MoveToEx(hdc, x, 0, NULL);
		LineTo(hdc, x, tableBottomY);
	}

	// Dispose objects
	SelectObject(hdc, oldPen);
	SelectObject(hdc, oldFont);
}

// Write text and draw horisontal lines
VOID WriteText(HDC hdc, RECT clientRect, TABLE table, INT columnWidth)
{
	for (int i = 0; i < table.numOfRows; i++)
	{
		WriteRow(hdc, clientRect, table, i, columnWidth);
	}
}

VOID WriteRow(HDC hdc, RECT clientRect, TABLE table, INT rowIndex, INT columnWidth)
{
	int maxY = tableBottomY;

	for (int i = 0; i < table.numOfColums; i++)
	{
		// Structure that receives dimensions of the string in logical units
		SIZE textSize;   
		// Get string to be written
		std::string strObj = table.text[rowIndex][i];
		// Convert std::string to LPSTR format
		std::wstring str(strObj.begin(), strObj.end());
		// Compute the width and the height of the string 
		GetTextExtentPoint32(hdc, str.c_str(), str.length(), &textSize);

		// The string fits into one line
		if (textSize.cx <= columnWidth - 2 * INDENT)
		{
			// Write the string to the specified coordinates
			int x = columnWidth * i + INDENT;
			int y = tableBottomY - scrolledWidth + INDENT;
			TextOut(hdc, x, y, str.c_str(), str.length());
			// Set new bottom coordinate
			if (tableBottomY + textSize.cy > maxY)
				maxY = tableBottomY + textSize.cy;
		}
		else 
		{
			SIZE stringSize;
			std::wstring substr = str;
			std::wstring restStr;

			int strWidth = textSize.cx;
			int restWidth;
			int strLength = str.length();

			// Current y-coordinate
			int y = tableBottomY + INDENT;

			do
			{
				int substrLen = GetNumOfCharsToWrite(hdc, columnWidth, str);
				substr = str.substr(0, substrLen);

				if (strLength >= substrLen)
				{
					restStr = str.substr(substrLen, strLength);
					GetTextExtentPoint32(hdc, substr.c_str(), substrLen, &stringSize);
					restWidth = strWidth - stringSize.cx;
					TextOut(hdc, columnWidth * i + INDENT, y - scrolledWidth, substr.c_str(), substr.length());

					y += stringSize.cy + INDENT;
					str = restStr;
					strLength = str.length();
					strWidth = restWidth;
				}
			} while (restWidth > columnWidth - 2 * INDENT);

			// Print rest of the string
			if (restWidth > 0)
				TextOut(hdc, columnWidth * i + INDENT, y - scrolledWidth, str.c_str(), str.length());

			// Set new bottom coordinate
			if (y + stringSize.cy > maxY) 
				maxY = y + stringSize.cy;
		}
	}

	tableBottomY = maxY + INDENT;

	// Draw horizontal line
	MoveToEx(hdc, clientRect.left, tableBottomY - scrolledWidth, NULL);
	LineTo(hdc, clientRect.right, tableBottomY - scrolledWidth);
}

// Return number of chars in one line in the column
INT GetNumOfCharsToWrite(HDC hdc, INT columnWidth, std::wstring str)
{
	SIZE stringSize;
	INT charCount = (columnWidth - 2 * INDENT) / CHAR_WIDTH;
	std::wstring substr = str.substr(0, charCount);
	GetTextExtentPoint32(hdc, substr.c_str(), substr.length(), &stringSize);

	// Correct charCount value
	while (stringSize.cx < columnWidth - 2 * INDENT && charCount < str.length())
	{
		charCount++;
		substr = str.substr(0, charCount);
		GetTextExtentPoint32(hdc, substr.c_str(), substr.length(), &stringSize);
	}
	while (stringSize.cx > columnWidth - 2 * INDENT)
	{
		charCount--;
		substr = str.substr(0, charCount);
		GetTextExtentPoint32(hdc, substr.c_str(), substr.length(), &stringSize);
	}

	// At least 1 character must be placed in the line
	if (charCount == 0) return 1;

	return charCount;
}

// Set the window to be redrawn
VOID UpdateTable(HWND hWnd)
{
	tableBottomY = 0;
	InvalidateRect(hWnd, NULL, TRUE);
}

// Checks whether the table needs to be redrawn if the user rotates the mouse wheel
BOOL IsScrolling(HWND hWnd, WPARAM wParam)
{
	// Contain dimentions of the window
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// Indicates the amount that the mouse wheel has changed
	SHORT delta = GET_WHEEL_DELTA_WPARAM(wParam);

	// The table is shorter than the window
	if (tableBottomY < clientRect.bottom)
		return false;

	// User is scrolling up but it's the upper position of the table
	if (scrolledWidth == 0 && delta > 0)
		return false;

	// The bottom border of the table coincides with the bottom border of the window 
	// and the user is scrolling the mouse wheel up
	if (scrolledWidth + clientRect.bottom == tableBottomY && delta < 0)
		return false;

	// Reached the bottom border of the table
	if (scrolledWidth + clientRect.bottom - delta >= tableBottomY && delta < 0)
	{
		scrolledWidth = tableBottomY - clientRect.bottom;
		return true;
	}

	// If the user is scrolling down, delta is negative
	scrolledWidth += (-delta);
	// Correct the value of scrolledY
	scrolledWidth = (scrolledWidth > 0) ? scrolledWidth : 0;

	return true;
}