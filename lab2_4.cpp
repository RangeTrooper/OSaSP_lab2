#include "stdafx.h"
#include "lab2_4.h"
#include "Table.h"
#include "TextProcessor.h"
#include "MyDlgWnd.h"

bool isSizeIncreasing = false;

#define MAX_LOADSTRING 150
#define ROW_COUNT 5
#define COLUMN_COUNT 6

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

RECT clientSize;
TextProcessor* textProcessor = new TextProcessor();
auto text = textProcessor->text;
Table* tableDrawer = new Table(ROW_COUNT, COLUMN_COUNT, text);
HWND currWindow;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY2OSISP, szWindowClass, MAX_LOADSTRING);
	//DialogBox()
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2OSISP));
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2OSISP));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY2OSISP);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; 

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	currWindow = hWnd;
	if (!hWnd)
	{
		return FALSE;
	}
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, MyWndProc);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		const auto color = RGB(0, 0, 0);
		const auto pen = CreatePen(PS_SOLID, 1, color);
		SelectObject(hdc, pen);

		tableDrawer->SetClientSize(clientSize);
		tableDrawer->RedrawTable(hdc,currWindow, isSizeIncreasing);

		DeleteObject(hdc);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_SIZE:
	{
		RECT oldClientSize = clientSize;
		GetClientRect(hWnd, &clientSize);
		if ((clientSize.right > oldClientSize.right) || (clientSize.bottom > oldClientSize.bottom))
			isSizeIncreasing = true;
		else
			isSizeIncreasing = false;
	}
	break;
	case WM_GETMINMAXINFO:
	{
		const auto lpMmi = reinterpret_cast<LPMINMAXINFO>(lParam);
		lpMmi->ptMinTrackSize.x = 700;
		lpMmi->ptMinTrackSize.y = 500;
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

