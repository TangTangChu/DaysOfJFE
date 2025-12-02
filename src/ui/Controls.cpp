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
    return x >= bounds.x && x < bounds.x + bounds.w && y >= bounds.y &&
           y < bounds.y + bounds.h;
}


int Controls::GetX() const { return bounds.x; }
int Controls::GetY() const { return bounds.y; }
int Controls::GetWidth() const { return bounds.w; }
int Controls::GetHeight() const { return bounds.h; }

void Controls::SetX(int x) { bounds.x = x; }
void Controls::SetY(int y) { bounds.y = y; }
void Controls::SetWidth(int w) { bounds.w = w; }
void Controls::SetHeight(int h) { bounds.h = h; }

void Controls::SetPosition(int x, int y) {
    bounds.x = x;
    bounds.y = y;
}

void Controls::SetSize(int w, int h) {
    bounds.w = w;
    bounds.h = h;
}

void Controls::SetBounds(int x, int y, int w, int h) {
    bounds.x = x;
    bounds.y = y;
    bounds.w = w;
    bounds.h = h;
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
