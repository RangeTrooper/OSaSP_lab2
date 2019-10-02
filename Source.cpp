#include <windows.h>  
#include <stdlib.h>  
#include <tchar.h>
#include <fstream>
#include <string>
#include "resource.h"

#define ROWS 4
#define COLUMNS 4
#define MAX_ROWS 5
#define MAX_COLUMNS 6
#define MIN_ROWS 2
#define MIN_COLUMNS 2
#define SCROLL_LINE_WIDTH 25
#define INDENT 10
#define FONT_NAME L"Consolas"

HDC hdc;
int windowWidth = 600, windowHeight = 200;
int colNumber = COLUMNS, rows = ROWS;
int x, y;
int columnWidth;
int shiftNum = 0;
int tableHeight;
int currentScrollPos = 0;
int posNum = 1;
int posPerPage;

PAINTSTRUCT ps;
HPEN linePen;
HGDIOBJ prevPen;
COLORREF backgroundColor = RGB(250, 250, 235);	// фон окна и текста
COLORREF textColor = RGB(50, 50, 30);	//цвет текста 
COLORREF penColor = RGB(80, 180, 70);	// цвет линий
HMENU menu, hPopupMenu1, hPopupMenu2;
FILE* file;

char* table[MAX_ROWS * MAX_COLUMNS];
LOGFONT logFont;
TEXTMETRIC textMetric;
HFONT hFont;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
int ShowModalDialogWnd(HINSTANCE);

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	ShowModalDialogWnd(hInstance);
	/*WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(backgroundColor);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("WindowClass");
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(
		_T("WindowClass"),
		_T("Window Application"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		HWND_DESKTOP,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	*/
	return 0;
}

VOID ReadFile() // заполнение таблицы из файла
{
	try
	{
		std::ifstream file("E:\\Downloads\\OSiSP-master\\OSiSP-master\\OSISPSecondLab\\WordTable.txt");
		if (file.is_open())
		{
			for (int i = 0; i < MAX_ROWS * MAX_COLUMNS; i++)
			{
				table[i] = new char[256];
				file.getline(table[i], 256);
			}
			file.close();
		}
	}
	catch (int i)
	{
		MessageBox(GetActiveWindow(), _T("Something bad happened..."), _T("Error"), MB_ICONERROR);
	}
}

void InitMyFont()
{
	logFont.lfHeight = 25;
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = FW_NORMAL;
	logFont.lfItalic = 0;
	logFont.lfUnderline = 0;
	logFont.lfStrikeOut = 0;
	logFont.lfCharSet = ANSI_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = PROOF_QUALITY;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN;
	wcscpy_s(logFont.lfFaceName, FONT_NAME);
	textMetric.tmAveCharWidth = 10;
	hFont = CreateFontIndirect(&logFont);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
	int wheelNum;
	int prevPos, prevPosNum;
	switch (message)
	{
	case WM_CREATE:
		//CreateMenu(hWnd);
		InitMyFont();
		ReadFile();
		linePen = CreatePen(PS_SOLID, 2, penColor);
		prevPen = SelectObject(hdc, linePen);
		currentScrollPos = 0;
		shiftNum = 0;
		SetScrollRange(hWnd, SB_VERT, 0, 0, FALSE);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		ShowTable(hWnd);
		EndPaint(hWnd, &ps);

		prevPosNum = posNum;
		posNum = GetPosNum(hWnd);

		SetScrollRange(hWnd, SB_VERT, 0, posNum - 1, FALSE);
		if (prevPosNum < posNum)
		{
			SetScrollPos(hWnd, SB_VERT, 0, TRUE);
		}
		break;
	case WM_GETMINMAXINFO:
		lpMMI->ptMinTrackSize.x = 200;
		lpMMI->ptMinTrackSize.y = 200;
		break;
	case WM_SIZE:
		windowHeight = HIWORD(lParam);
		windowWidth = LOWORD(lParam);

		posNum = GetPosNum(hWnd);
		currentScrollPos = 0;
		shiftNum = 0;
		SetScrollPos(hWnd, SB_VERT, currentScrollPos, TRUE);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_VSCROLL:
		prevPos = currentScrollPos;
		switch (wParam) {
		case SB_PAGEUP:
			posPerPage = windowHeight / SCROLL_LINE_WIDTH;
			if (currentScrollPos - posPerPage <= 0)  // если сверху меньше, чем страница
			{
				currentScrollPos = 0;
				shiftNum = 0;
			}
			else // если сверху еще больше, чем страница 
			{
				currentScrollPos -= posPerPage;
				shiftNum = -currentScrollPos * SCROLL_LINE_WIDTH;
			}
			SetScrollPos(hWnd, SB_VERT, currentScrollPos, TRUE);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case SB_PAGEDOWN:
			posPerPage = windowHeight / SCROLL_LINE_WIDTH;
			if (currentScrollPos + posPerPage >= posNum - 1)  // если снизу меньше, чем страница
			{
				currentScrollPos = posNum - 1;
			}
			else // если снизу еще больше, чем страница 
			{
				currentScrollPos += posPerPage;
			}
			shiftNum = -currentScrollPos * SCROLL_LINE_WIDTH;
			SetScrollPos(hWnd, SB_VERT, currentScrollPos, TRUE);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case SB_LINEUP:
			if (currentScrollPos > 0 && posNum > 1)
			{
				currentScrollPos--;
				shiftNum += SCROLL_LINE_WIDTH;
			}
			break;
		case SB_LINEDOWN:
			if (posNum > 1 && currentScrollPos < posNum - 1)
			{
				currentScrollPos++;
				shiftNum -= SCROLL_LINE_WIDTH;
			}
			break;
		}
		RedrawIfChange(hWnd, prevPos);
		break;
	case WM_MOUSEWHEEL:
	{
		prevPos = currentScrollPos;
		wheelNum = GET_WHEEL_DELTA_WPARAM(wParam);

		if (wheelNum > 0) // up
		{
			if (currentScrollPos > 0 && posNum > 1)
			{
				shiftNum += SCROLL_LINE_WIDTH;
				currentScrollPos--;
			}
		}
		else // down
		{
			if (posNum > 1 && currentScrollPos < posNum - 1)
			{
				shiftNum -= SCROLL_LINE_WIDTH;
				currentScrollPos++;
			}
		}
		RedrawIfChange(hWnd, prevPos);
	}
	break;
	case WM_COMMAND:
		switch (wParam)
		{
		case 11:
			if (rows < MAX_ROWS)
				rows++;
			break;
		case 12:
			if (rows > MIN_ROWS)
				rows--;
			break;
		case 21:
			if (colNumber < MAX_COLUMNS)
				colNumber++;
			break;
		case 22:
			if (colNumber > MIN_COLUMNS)
				colNumber--;
			break;
		}
		currentScrollPos = 0;
		SetScrollPos(hWnd, SB_VERT, currentScrollPos, TRUE);
		shiftNum = 0;
		posNum = GetPosNum(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		SelectObject(hdc, prevPen);
		DeleteObject(linePen);
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}


void ShowTable(HWND hWnd)
{
	columnWidth = (windowWidth / colNumber);
	RECT cellRect;
	y = 0;
	int count = 0;

	for (int i = 0; i < rows; i++)
	{
		int h = 1;
		x = 0;
		int hMax = 0;
		SelectObject(hdc, linePen);
		MoveToEx(hdc, 0, y + shiftNum + 1, NULL);
		LineTo(hdc, colNumber * columnWidth, y + shiftNum + 1);

		for (int j = 0; j < colNumber; j++)
		{
			SetRect(&cellRect, x, y + INDENT + shiftNum, x + columnWidth, y + 1000 + shiftNum);
			SetBkColor(hdc, backgroundColor);
			SelectObject(hdc, hFont);
			SetTextColor(hdc, textColor);
			h = DrawTextA(hdc, table[count], strlen(table[count]), &cellRect, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
			count++;
			x += columnWidth;

			if (h > hMax)
				hMax = h;
		}

		y += hMax + INDENT * 2;

		SelectObject(hdc, linePen);
		MoveToEx(hdc, 0, y + shiftNum, NULL);
		LineTo(hdc, colNumber * columnWidth, y + shiftNum);
	}

	MoveToEx(hdc, 0, y + shiftNum, NULL);
	LineTo(hdc, colNumber * columnWidth, y + shiftNum);
	tableHeight = y;

	x = 0;
	for (int i = 0; i <= colNumber; i++) {

		SelectObject(hdc, linePen);
		MoveToEx(hdc, x, 0 + shiftNum, NULL);
		LineTo(hdc, x, y + shiftNum);
		x += columnWidth;
	}
}

int ShowModalDialogWnd(HINSTANCE hInst)
{
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, (DlgProc), 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam)
{
	switch (uint)
	{
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDOK:
			EndDialog(hwnd, 0);
			break;
		case IDCANCEL:
			return EndDialog(hwnd, 0);
		default:
			return 2;
		}
		break;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;

	}
	//Edit_GetText();
	return false;
}

int GetPosNum(HWND hWnd) // количество позиций
{
	if (windowHeight < tableHeight)
	{
		float d = ((float)(tableHeight - windowHeight) / (float)SCROLL_LINE_WIDTH) + 1;
		return ceil(d);
	}
	else
	{
		currentScrollPos = 0;
		shiftNum = 0;
		return 1;
	}
}

void RedrawIfChange(HWND hWnd, int prevPos)
{
	if (prevPos != currentScrollPos) // если позиция изменилась
	{
		SetScrollPos(hWnd, SB_VERT, currentScrollPos, TRUE);
		InvalidateRect(hWnd, NULL, TRUE);
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
	int wheelNum;
	int prevPos, prevPosNum;
	switch (message)
	{
	case WM_CREATE:
		//CreateMenu(hWnd);
		InitMyFont();
		ReadFile();
		linePen = CreatePen(PS_SOLID, 2, penColor);
		prevPen = SelectObject(hdc, linePen);
		currentScrollPos = 0;
		shiftNum = 0;
		SetScrollRange(hWnd, SB_VERT, 0, 0, FALSE);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		ShowTable(hWnd);
		EndPaint(hWnd, &ps);

		prevPosNum = posNum;
		posNum = GetPosNum(hWnd);

		SetScrollRange(hWnd, SB_VERT, 0, posNum - 1, FALSE);
		if (prevPosNum < posNum)
		{
			SetScrollPos(hWnd, SB_VERT, 0, TRUE);
		}
		break;
	case WM_GETMINMAXINFO:
		lpMMI->ptMinTrackSize.x = 200;
		lpMMI->ptMinTrackSize.y = 200;
		break;
	case WM_SIZE:
		windowHeight = HIWORD(lParam);
		windowWidth = LOWORD(lParam);

		posNum = GetPosNum(hWnd);
		currentScrollPos = 0;
		shiftNum = 0;
		SetScrollPos(hWnd, SB_VERT, currentScrollPos, TRUE);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_VSCROLL:
		prevPos = currentScrollPos;
		switch (wParam) {
		case SB_PAGEUP:
			posPerPage = windowHeight / SCROLL_LINE_WIDTH;
			if (currentScrollPos - posPerPage <= 0)  // если сверху меньше, чем страница
			{
				currentScrollPos = 0;
				shiftNum = 0;
			}
			else // если сверху еще больше, чем страница 
			{
				currentScrollPos -= posPerPage;
				shiftNum = -currentScrollPos * SCROLL_LINE_WIDTH;
			}
			SetScrollPos(hWnd, SB_VERT, currentScrollPos, TRUE);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case SB_PAGEDOWN:
			posPerPage = windowHeight / SCROLL_LINE_WIDTH;
			if (currentScrollPos + posPerPage >= posNum - 1)  // если снизу меньше, чем страница
			{
				currentScrollPos = posNum - 1;
			}
			else // если снизу еще больше, чем страница 
			{
				currentScrollPos += posPerPage;
			}
			shiftNum = -currentScrollPos * SCROLL_LINE_WIDTH;
			SetScrollPos(hWnd, SB_VERT, currentScrollPos, TRUE);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case SB_LINEUP:
			if (currentScrollPos > 0 && posNum > 1)
			{
				currentScrollPos--;
				shiftNum += SCROLL_LINE_WIDTH;
			}
			break;
		case SB_LINEDOWN:
			if (posNum > 1 && currentScrollPos < posNum - 1)
			{
				currentScrollPos++;
				shiftNum -= SCROLL_LINE_WIDTH;
			}
			break;
		}
		RedrawIfChange(hWnd, prevPos);
		break;
	case WM_MOUSEWHEEL:
	{
		prevPos = currentScrollPos;
		wheelNum = GET_WHEEL_DELTA_WPARAM(wParam);

		if (wheelNum > 0) // up
		{
			if (currentScrollPos > 0 && posNum > 1)
			{
				shiftNum += SCROLL_LINE_WIDTH;
				currentScrollPos--;
			}
		}
		else // down
		{
			if (posNum > 1 && currentScrollPos < posNum - 1)
			{
				shiftNum -= SCROLL_LINE_WIDTH;
				currentScrollPos++;
			}
		}
		RedrawIfChange(hWnd, prevPos);
	}
	break;
	case WM_COMMAND:
		switch (wParam)
		{
		case 11:
			if (rows < MAX_ROWS)
				rows++;
			break;
		case 12:
			if (rows > MIN_ROWS)
				rows--;
			break;
		case 21:
			if (colNumber < MAX_COLUMNS)
				colNumber++;
			break;
		case 22:
			if (colNumber > MIN_COLUMNS)
				colNumber--;
			break;
		}
		currentScrollPos = 0;
		SetScrollPos(hWnd, SB_VERT, currentScrollPos, TRUE);
		shiftNum = 0;
		posNum = GetPosNum(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		SelectObject(hdc, prevPen);
		DeleteObject(linePen);
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}


