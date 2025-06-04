#pragma once
#include "Controls.h"
#include "functional"
class ClickPaper :public Controls {
public:
	ClickPaper(std::function<void()> onClick) {
		this->OnMouseDownHook = onClick;
	}
	void Draw(HDC hdc) {};
	void OnMouseUp(int x, int y);
};