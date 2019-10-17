#pragma once
#include <vector>
#include <string>

typedef std::vector<std::vector<std::wstring>> TStringsMatrix;
typedef std::vector<std::wstring> TStringList;

class TextProcessor
{
private:
	std::wstring ConvertString(std::string temp);
public:
	TStringsMatrix text;
	TextProcessor();
};

