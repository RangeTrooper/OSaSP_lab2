#pragma once
#include "TextProcessor.h"
#include "wtypes.h"

#define MAX_ROWS 11
#define MAX_COLUMNS 11

class Table
{
	const int _horizontalTab = 0;
	int _maxRows = MAX_ROWS;
	int _maxColumns = MAX_COLUMNS;
	RECT _clientSize;
	TStringsMatrix _phrases;
	int _fontHeight = 36, _verticalGap = 0, _totalHeight = 0;
	int _rowCount = 0, _columnCount = 0;
	int _horizontalBorders[MAX_ROWS][2];
	int _verticalBorders[MAX_COLUMNS][2];


	void DrawCellBorders(HDC hDc);
	void DrawTable(HDC hDc, bool isDraw);
	void PrintPhrases(HDC hDc);
	int RecalculateVerticalBorders();
	void CalculateHorizontalBorders();
	int CalculateVerticalBorders(HDC hDc);

public:
	Table(int rowCount, int columnCount, TStringsMatrix phrases);

	void RedrawTable(HDC hDc, HWND hwnd, bool isSizeIncreasing);
	void SetClientSize(RECT clientSize);
};

