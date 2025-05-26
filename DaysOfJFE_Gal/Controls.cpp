#include "stdafx.h"
#include "Controls.h"

void Controls::OnMouseDown(int x, int y)
{
	if (OnMouseDownHook) {
		OnMouseDownHook();
	}
}
void Controls::OnMouseUp(int x, int y)
{
	if (OnMouseUpPreHook) {
		OnMouseDownHook();
	}
	if (onClick) {
		onClick();
	}
	if (OnMouseUpAftHook) {
		OnMouseUpAftHook();
	}
}
void Controls::OnMouseReset()
{
	if (OnMouseResetHook) {
		OnMouseResetHook();
	}
}
void Controls::SetOnClick(std::function<void()> onClick) { this->onClick = onClick; }
void Controls::SetOnMouseUpPreHook(std::function<void()> OnMouseUpPreHook) { this->OnMouseUpPreHook = OnMouseUpPreHook; }
void Controls::SetOnMouseUpAftHook(std::function<void()> OnMouseUpAftHook) { this->OnMouseUpAftHook = OnMouseUpAftHook; }
void Controls::SetOnMouseDownHook(std::function<void()> OnMouseDownHook) { this->OnMouseDownHook = OnMouseDownHook; }
void Controls::SetOnMouseResetHook(std::function<void()> OnMouseResetHook) { this->OnMouseResetHook = OnMouseResetHook; }
Rect Controls::GetBounds() const {
	return { X, Y, Width, Height };
}
bool Controls::Contains(int x, int y) const {
	return x >= X && x <= X + Width &&
		y >= Y && y <= Y + Height;
}

void Controls::SetPosition(int x, int y) {
	X = x;
	Y = y;
	this->rect = Rect{ X,Y,Width,Height };
}

void Controls::SetSize(int width, int height) {
	Width = width;
	Height = height;
	this->rect = Rect{ X,Y,Width,Height };
}
void Controls::SetRect(Rect rect) {
	this->rect = rect;
	this->X = rect.X;
	this->Y = rect.Y;
	this->Height = rect.Height;
	this->Width = rect.Width;
}
void Controls::SetVisible(bool v) { visible = v; }
bool Controls::IsVisible() const { return visible; }

void Controls::SetEnabled(bool e) { enabled = e; }
bool Controls::IsEnabled() const { return enabled; }

void Controls::SetFontSize(int fontsize) {
	this->fontSize = fontsize;
}
void Controls::SetFontStyle(int fontstyle) {
	this->fontStyle = fontstyle;
}