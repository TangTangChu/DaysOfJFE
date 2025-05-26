#pragma once
#include "Controls.h"
#include <String>
#include <gdiplus.h>
class TitleTransition :public Controls {
private:
	std::wstring Text;
public:
	TitleTransition(std::wstring text =L"БъЬт");
	void SetText(std::wstring text);
	void Draw(HDC hdc) override;
};