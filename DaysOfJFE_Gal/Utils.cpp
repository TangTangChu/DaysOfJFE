#include "stdafx.h"
#include "Utils.h"

std::wstring Utils::utf8ToWide(const std::string& str) {
	if (str.empty()) return L"";
	const int size_needed = MultiByteToWideChar(
		CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(
		CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
	return wstr;
}

std::string Utils::wideToUtf8(const std::wstring& wstr) {
	if (wstr.empty()) return "";
	const int size_needed = WideCharToMultiByte(
		CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
	std::string str(size_needed, 0);
	WideCharToMultiByte(
		CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
	return str;
}