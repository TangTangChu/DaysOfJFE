#pragma once
#include "Controls.h"
#include "string"
#include "memory"
#include <gdiplus.h>
#include <thread>
#include <atomic>
#include <mutex>
using namespace Gdiplus;
class TextBlock :public Controls {
protected:
	std::unique_ptr<StringFormat> format;
private:
	std::wstring Text;
	std::wstring ShowText;
	Color TextColor;
	FontFamily fontFamily;
	std::atomic<bool> m_running{ false };
	std::thread m_thread;
	std::mutex m_mutex;
	bool m_loopAnimation = false;
public:
	TextBlock(const WCHAR* text, int fontsize = 14,
		Color textColor = Color(255, 255, 255, 255)) :
		Text(text), TextColor(textColor), ShowText(text), fontFamily(L"Î¢ÈíÑÅºÚ"), format(nullptr)
	{
		this->fontSize = fontsize;
	};
	void Draw(HDC hdc) override;
	void SetStringFormat(std::unique_ptr<StringFormat> newFormat);
	void SetText(const std::wstring& newText);
	bool IsAnimating() const;
	void TextAnimation();
	void startAnimation(bool loop = false);
	void stopAnimation();
	void showFullText();
	void clearText();
};