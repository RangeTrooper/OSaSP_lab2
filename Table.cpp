#include "stdafx.h"
#include "Table.h"


Table::Table(int rowCount, int columnCount, TStringsMatrix phrases)
{
	if (rowCount > _maxRows)
	{
		_rowCount = _maxRows;
	}
	else
	{
		_rowCount = rowCount;
	}

	if (columnCount > _maxColumns)
	{
		_columnCount = _maxColumns;
	}
	else
	{
		_columnCount = columnCount;
	}
	_phrases = phrases;
}

void Table::SetClientSize(RECT clientSize)
{
	if ((clientSize.right < _horizontalBorders[_columnCount][1]))
		_clientSize.right = _horizontalBorders[_columnCount][1] + 20;
	else
		_clientSize.right = clientSize.right;
	if (clientSize.bottom < _verticalBorders[_rowCount][1])
		_clientSize.bottom = _verticalBorders[_rowCount][1];
	else
		_clientSize.bottom = clientSize.bottom;
	_clientSize.top = clientSize.top;
	_clientSize.left = clientSize.left;

}

void Table::RedrawTable(HDC hDc, HWND hwnd, bool isSizeIncreasing)
{
	_verticalGap = 0;
	CalculateHorizontalBorders();
	DrawTable(hDc, false);

	while (_clientSize.bottom - _totalHeight > 0)
	{
		_fontHeight++;
		CalculateHorizontalBorders();
		DrawTable(hDc, false);
	}
	while (_clientSize.bottom - _totalHeight > 0)
	{
		_totalHeight = RecalculateVerticalBorders();
		_verticalGap++;
	}
	while (_totalHeight >= _clientSize.bottom)
	{
		if (_fontHeight > 15)
			_fontHeight--;
		else if (!isSizeIncreasing)
			_fontHeight = 15;
		CalculateHorizontalBorders();
		DrawTable(hDc, false);
	}
	DrawTable(hDc, true);
}

void Table::DrawCellBorders(HDC hDc)
{
	for (int i = 0; i < _rowCount - 1; i++)
	{
		MoveToEx(hDc, _clientSize.left, _verticalBorders[i][1], nullptr);
		LineTo(hDc, _clientSize.right, _verticalBorders[i][1]);
	}
	for (int i = 0; i < _columnCount - 1; i++)
	{
		MoveToEx(hDc, _horizontalBorders[i][1], 0, nullptr);
		LineTo(hDc, _horizontalBorders[i][1], _clientSize.bottom);
	}
}

void Table::PrintPhrases(HDC hDc)
{
	RECT rect;

	for (int i = 0; i < _rowCount; i++)
	{
		for (int j = 0; j < _columnCount; j++)
		{
			rect.top = _verticalBorders[i][0] + _verticalGap / 2;
			rect.bottom = _verticalBorders[i][1] - _verticalGap / 2;
			rect.left = _horizontalBorders[j][0] + _verticalGap / 2;
			rect.right = _horizontalBorders[j][1] - _verticalGap / 2;

			DrawText(hDc, (_phrases[i][j].c_str()), -1, &rect, DT_WORDBREAK);
		}
	}
}

int Table::CalculateVerticalBorders(HDC hDc)
{
	RECT rect;
	rect.top = 0;

	int totalHeight = 0;
	_verticalBorders[0][0] = 0;
	for (int i = 0; i < _rowCount; i++)
	{
		if (i > 0)
		{
			_verticalBorders[i][0] = _verticalBorders[i - 1][1];
		}
		int rowMaxHeight = 0;
		for (int j = 0; j < _columnCount; j++)
		{
			rect.bottom = 0;
			rect.left = _horizontalBorders[j][0];
			rect.right = _horizontalBorders[j][1];

			DrawText(hDc, (_phrases[i][j].c_str()), -1, &rect, DT_CALCRECT | DT_WORDBREAK | DT_WORD_ELLIPSIS);
			rowMaxHeight = max(rowMaxHeight, rect.bottom);
		}
		totalHeight += rowMaxHeight;
		if (i > 0)
		{
			_verticalBorders[i][1] = _verticalBorders[i - 1][1] + rowMaxHeight;
		}
		else
		{
			_verticalBorders[0][1] = rowMaxHeight;
		}
	}
	return totalHeight;
}

int Table::RecalculateVerticalBorders()
{
	auto counter = 1;
	auto totalHeight = 0;
	for (int i = 0; i < _rowCount; i++)
	{
		if (i > 0)
		{
			_verticalBorders[i][0] += counter;
		}
		_verticalBorders[i][1] += counter;

		counter++;
	}
	totalHeight += _verticalBorders[_rowCount - 1][1];

	return totalHeight;
}

void Table::CalculateHorizontalBorders()
{
	const auto step = (_clientSize.right - _clientSize.left) / _columnCount;
	for (int i = 0; i < _columnCount; i++)
	{
		_horizontalBorders[i][0] = i * step;
		_horizontalBorders[i][1] = (i + 1) * step;
	}
}

void Table::DrawTable(HDC hDc, bool isDraw)
{
	auto font = CreateFont(_fontHeight, 0, 0, 0, FW_NORMAL, true, false, false, 0, 0, 0, 0, FF_ROMAN, nullptr);
	SelectObject(hDc, font);
	if (isDraw)
	{
		PrintPhrases(hDc);
		DrawCellBorders(hDc);
	}
	else
	{
		_totalHeight = CalculateVerticalBorders(hDc);
	}
	DeleteObject(font);

}