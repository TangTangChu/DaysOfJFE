#pragma once
#include <gdiplus.h>
#include "Controls.h"
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <iostream>
using namespace Gdiplus;

class OutlineText : public Controls {
private:
	int fontSize;
	int outlineWidth;
	std::wstring text;
	std::wstring showtext = L"";
	Color outlineColor;
	Color textColor;
	std::atomic<bool> m_running{ false };
	std::thread m_thread;
	std::mutex m_mutex;
	bool m_loopAnimation = false; //  «∑Ò—≠ª∑∂Øª≠

public:
	OutlineText(const WCHAR* text, int fontsize = 14, int thick = 1,
		Color outlineColor = Color(255, 25, 25, 25),
		Color textColor = Color(255, 255, 255, 255)) :
		text(text), fontSize(fontsize), outlineWidth(thick),
		outlineColor(outlineColor), textColor(textColor),showtext(text){}

	~OutlineText() {
		stopAnimation(); 
	}

	void SetText(const std::wstring& newText);
	bool IsAnimating() const;
	void Draw(HDC hdc) override;

	void TextAnimation();

	void startAnimation(bool loop = false);

	void stopAnimation();

	void showFullText();

	void clearText();
};
