#include "Controls.h"

void Controls::OnMouseUp(int x, int y) {
    if (!IsVisible() || !IsEnabled()) return;
    
    if (Contains(x, y)) {
        if (OnMouseUpPreHook) OnMouseUpPreHook();
        if (onClick) onClick();
        if (OnMouseUpAftHook) OnMouseUpAftHook();
    }
}

void Controls::OnMouseDown(int x, int y) {
    if (!IsVisible() || !IsEnabled()) return;
    
    if (Contains(x, y)) {
        if (OnMouseDownHook) OnMouseDownHook();
    }
}

void Controls::OnMouseReset() {
    if (OnMouseResetHook) OnMouseResetHook();
}

void Controls::HandleEvent(const SDL_Event& event) {
    if (!IsVisible() || !IsEnabled()) return;
    
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        OnMouseDown(event.button.x, event.button.y);
    } else if (event.type == SDL_MOUSEBUTTONUP) {
        OnMouseUp(event.button.x, event.button.y);
    }
}

SkRect Controls::GetBounds() const {
    return SkRect::MakeXYWH(X, Y, Width, Height);
}

bool Controls::Contains(int x, int y) const {
    return x >= X && x <= X + Width && y >= Y && y <= Y + Height;
}

bool Controls::IsPointInside(int x, int y) const {
    return Contains(x, y);
}

void Controls::SetPosition(int x, int y) {
    X = x;
    Y = y;
    rect = SkRect::MakeXYWH(X, Y, Width, Height);
}

void Controls::SetSize(int width, int height) {
    Width = width;
    Height = height;
    rect = SkRect::MakeXYWH(X, Y, Width, Height);
}

void Controls::SetRect(SkRect r) {
    rect = r;
    X = r.x();
    Y = r.y();
    Width = r.width();
    Height = r.height();
}

void Controls::SetFontSize(int fontsize) {
    fontSize = fontsize;
}

void Controls::SetFontStyle(int fontstyle) {
    fontStyle = fontstyle;
}

void Controls::SetVisible(bool v) {
    visible = v;
}

bool Controls::IsVisible() const {
    return visible;
}

void Controls::SetEnabled(bool e) {
    enabled = e;
}

bool Controls::IsEnabled() const {
    return enabled;
}

void Controls::SetOnClick(std::function<void()> func) {
    onClick = func;
}

void Controls::SetOnMouseUpPreHook(std::function<void()> func) {
    OnMouseUpPreHook = func;
}

void Controls::SetOnMouseUpAftHook(std::function<void()> func) {
    OnMouseUpAftHook = func;
}

void Controls::SetOnMouseDownHook(std::function<void()> func) {
    OnMouseDownHook = func;
}

void Controls::SetOnMouseResetHook(std::function<void()> func) {
    OnMouseResetHook = func;
}
