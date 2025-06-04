#pragma once
#include <functional>
#include <gdiplus.h>
#include <windows.h>
using namespace Gdiplus;

class Controls {
protected:
	int X = 0, Y = 0, Width = 100, Height = 40;
	int fontStyle = 0, fontSize = 12;
	bool visible = true;
	bool enabled = true;
	Rect rect;
	std::function<void()> onClick;
	std::function<void()> OnMouseUpPreHook;
	std::function<void()> OnMouseUpAftHook;
	std::function<void()> OnMouseDownHook;
	std::function<void()> OnMouseResetHook;
	std::function<void()> OnSwitch;
public:
	virtual ~Controls() = default;
	virtual void Draw(HDC hdc) = 0;
	virtual void OnMouseUp(int x, int y);
	virtual void OnMouseDown(int x, int y);
	virtual void OnMouseReset();
	Rect GetBounds() const;

	bool Contains(int x, int y) const;

	void SetPosition(int x, int y);
	void SetSize(int width, int height);
	void SetRect(Rect rect);
	void SetFontSize(int fontsize);
	void SetFontStyle(int fontstyle);

	void SetVisible(bool v);
	bool IsVisible() const;

	void SetEnabled(bool e);
	bool IsEnabled() const;

	void SetOnClick(std::function<void()> onClick);
	void SetOnMouseUpPreHook(std::function<void()> OnMouseUpPreHook);
	void SetOnMouseUpAftHook(std::function<void()> OnMouseUpAftHook);
	void SetOnMouseDownHook(std::function<void()> OnMouseDownHook);
	void SetOnMouseResetHook(std::function<void()> OnMouseResetHook);
};
