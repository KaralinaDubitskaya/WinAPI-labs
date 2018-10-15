// Lab2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lab2.h"

#define MAX_LOADSTRING 100

typedef std::vector<std::string> StringVector;
typedef std::vector<StringVector> StringTable;
typedef struct _TABLE
{
	INT numOfColums = 0;                           // Number of colums in the table
	StringTable text;    // "Two-dimensional array" of strings to be dispalayed as a table 
} TABLE;

// Global Variables:
HINSTANCE hInst;                                // Current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // The main window class name
WCHAR szFileName[MAX_LOADSTRING];               // Name of the file with strings to be showed in the table
TABLE table;                                    // Struct that contains all info about table

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Edit(HWND, UINT, WPARAM, LPARAM);
VOID			    GetUserFileName(HWND, WCHAR []);
BOOL				LoadTextFromFile(WCHAR szFileName[], StringTable *table, int numOfColumns);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

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
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
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
					DialogBox(hInst, MAKEINTRESOURCE(IDD_EDITBOX), hWnd, Edit);
					// Load lines from the file to the table.text
					LoadTextFromFile(szFileName, &(table.text), table.numOfColums);
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
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
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
	// String which contains entered string
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
				MessageBox(hDlg, L"Too many colums.", L"Error",	MB_OK);
				EndDialog(hDlg, TRUE);
				return FALSE;
			}
			else if (cchColums == 0)
			{
				MessageBox(hDlg, L"No characters entered.",	L"Error", MB_OK);
				EndDialog(hDlg, TRUE);
				return FALSE;
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
				MessageBox(hDlg, L"Invalid input.", L"Error", MB_OK);
				EndDialog(hDlg, TRUE);
				return FALSE;
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
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Text\0*.txt\0"; // only text files
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Show the open file dialog box
	GetOpenFileName(&ofn);
	// Copy the file name to the szFileName
	wcscpy_s(szFileName, wcslen(szFile) + 1, szFile);
}

// Read the text file and put it's lines to the StringTable
BOOL LoadTextFromFile(WCHAR szFileName[], StringTable *table, int numOfColumns)
{
	// Convert wchar_t* to std::string
	std::wstring ws(szFileName);
	std::string fileName(ws.begin(), ws.end());

	// Create internal file stream
	std::ifstream fin(fileName.c_str());
	if (!fin)
		return false;

	std::string str;
	StringVector row;

	// Read the file line by line
	while (!fin.eof())
	{
		for (int i = 0; i < numOfColumns; i++)
		{
			// Read the next line
			std::getline(fin, str);
			// Add it to the row
			row.push_back(str);

			if (fin.eof()) 
				break;
		}

		// Add row to the table text
		(*table).push_back(row);
		row.clear();
	}

	fin.close();
	return true;
}

