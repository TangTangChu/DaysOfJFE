#include "ui/Controls.h"

void Controls::OnMouseDown(int, int) {
    if (!enabled)
        return;
    if (onMouseDownHook)
        onMouseDownHook();
}

void Controls::OnMouseUp(int, int) {
    if (!enabled)
        return;
    if (onMouseUpPreHook)
        onMouseUpPreHook();
    if (onClick)
        onClick();
    if (onMouseUpAftHook)
        onMouseUpAftHook();
}

void Controls::OnMouseReset() {
    if (onMouseResetHook)
        onMouseResetHook();
}

bool Controls::Contains(int x, int y) const {
    return x >= rect.X && x < rect.X + rect.Width && y >= rect.Y &&
           y < rect.Y + rect.Height;
}

const Rect &Controls::GetRect() const { return rect; }
void Controls::SetRect(Rect r) { rect = r; }

int Controls::GetX() const { return rect.X; }
int Controls::GetY() const { return rect.Y; }
int Controls::GetWidth() const { return rect.Width; }
int Controls::GetHeight() const { return rect.Height; }

void Controls::SetX(int x) { rect.X = x; }
void Controls::SetY(int y) { rect.Y = y; }
void Controls::SetWidth(int w) { rect.Width = w; }
void Controls::SetHeight(int h) { rect.Height = h; }

void Controls::SetPosition(int x, int y) {
    rect.X = x;
    rect.Y = y;
}

void Controls::SetSize(int w, int h) {
    rect.Width = w;
    rect.Height = h;
}

void Controls::SetBounds(int x, int y, int w, int h) {
    rect.X = x;
    rect.Y = y;
    rect.Width = w;
    rect.Height = h;
}

int Controls::GetFontSize() const { return fontSize; }
void Controls::SetFontSize(int fontsize) { fontSize = fontsize; }

int Controls::GetFontStyle() const { return fontStyle; }
void Controls::SetFontStyle(int fontstyle) { fontStyle = fontstyle; }

void Controls::SetVisible(bool v) { visible = v; }
bool Controls::IsVisible() const { return visible; }

void Controls::SetEnabled(bool e) { enabled = e; }
bool Controls::IsEnabled() const { return enabled; }

void Controls::SetOnClick(std::function<void()> click) {
    onClick = std::move(click);
}
void Controls::SetOnMouseUpPreHook(std::function<void()> hook) {
    onMouseUpPreHook = std::move(hook);
}
void Controls::SetOnMouseUpAftHook(std::function<void()> hook) {
    onMouseUpAftHook = std::move(hook);
}
void Controls::SetOnMouseDownHook(std::function<void()> hook) {
    onMouseDownHook = std::move(hook);
}
void Controls::SetOnMouseResetHook(std::function<void()> hook) {
    onMouseResetHook = std::move(hook);
}
