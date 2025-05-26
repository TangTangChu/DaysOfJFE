#pragma once
#include "Controls.h"
#include <functional>
#include <string>
#include <gdiplus.h>
#include <graphics.h>
using namespace Gdiplus;

struct CornerRadius {
	int A;
	int B;
	int C;
	int D;
};

class Button : public Controls {
protected:
	std::wstring text;
	Color textColor;
	CornerRadius radius{ 20,20,20,20 };
	Button(const std::wstring& text, std::function<void()> onClick, Color textColor)
		: text(text), textColor(textColor) {
		this->onClick = onClick;
		fontStyle = 1;
	}

public:
	void Draw(HDC hdc) = 0;
	void SetText(const std::wstring& newText);
	virtual ~Button() = default;
};
class PrimaryButton :public Button {
private:
	Color btnColor;
	Color hoverBgColor = Color(255, 255, 0, 127);
	Color FinaBrush;
	int cornerRadius = 20;
public:
	// 基础按钮构造函数，可以设置背景色和文本色
	PrimaryButton(const std::wstring& text,
		std::function<void()> onClick,
		Color textColor = Color(255, 255, 255, 255),
		Color backgroundColor = Color(255, 0, 191, 255))
		: Button(text, onClick, textColor), btnColor(backgroundColor), FinaBrush(backgroundColor) {
		this->OnMouseDownHook = [this]() {
			FinaBrush = hoverBgColor;
			};
		this->OnMouseUpPreHook = [this]() {
			FinaBrush = btnColor;
			};
		this->OnMouseResetHook = [this]() {
			FinaBrush = btnColor;
			};
	};
	void setBackgroundColor(Color color);
	void setHoverBgColor(Color color);
	void SetCornerRadius(int r);
	void SetCornerRadius(CornerRadius rad);
	void Draw(HDC hdc);
};

class TextButton :public Button {
public:
	TextButton(const std::wstring& text,
		std::function<void()> onClick,
		Color textColor = Color(255, 0, 153, 153))
		: Button(text, onClick, textColor) {
	}
	void Draw(HDC hdc);
};