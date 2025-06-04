#pragma once
#include <string>

class Utils {
public:
	static std::wstring utf8ToWide(const std::string& str);

	static std::string wideToUtf8(const std::wstring& wstr);
};