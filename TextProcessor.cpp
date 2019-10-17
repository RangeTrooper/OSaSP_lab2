#include "stdafx.h"
#include "TextProcessor.h"
#include "ctime"
#include "Table.h"
#include <fstream>

using namespace std;

std::wstring TextProcessor::ConvertString(string temp)
{
	std::wstring strRand = L"abcdefghijklmnopqrstuvwxyz";
	std::wstring newstr;
	int pos;
	std::wstring str2(50, L' '); // Make room for characters
	temp.resize(50);
	std::copy(temp.begin(), temp.end(), str2.begin());
	return str2;
}

TextProcessor::TextProcessor()
{
	TStringList textRow;

	//srand(time(NULL));
	std::ifstream file("E:\\Downloads\\OSiSP-master\\OSiSP-master\\OSISPSecondLab\\WordTable.txt");
	if (file.is_open())
	{
		for (int i = 0; i < MAX_ROWS; i++)
		{
			for (int j = 0; j < MAX_COLUMNS; j++)
			{
				string temp;
				getline(file, temp);
				textRow.push_back(ConvertString(temp).c_str());
			}
			text.push_back(textRow);
			textRow.clear();
		}
	}
}