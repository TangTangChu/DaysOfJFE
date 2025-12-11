#include "Layout.h"

LayoutControl::LayoutControl() {}

void LayoutControl::AddChild(std::shared_ptr<Controls> child) {
    if (child) {
        children.push_back(child);
        UpdateLayout();
    }
}

void LayoutControl::RemoveChild(std::shared_ptr<Controls> child) {
    auto it = std::remove(children.begin(), children.end(), child);
    if (it != children.end()) {
        children.erase(it, children.end());
        UpdateLayout();
    }
}

void LayoutControl::ClearChildren() {
    children.clear();
    pressedChild.reset();
    hoveredChild.reset();
}

void LayoutControl::Draw(IRenderer &r) {
    for (auto &c : children) {
        if (c && c->IsVisible()) {
            c->Draw(r);
        }
    }
}

Controls *LayoutControl::FindChildAt(int x, int y) {
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        const auto &c = *it;
        if (c && c->IsVisible() && c->IsEnabled() && c->Contains(x, y)) {
            return c.get();
        }
    }
    return nullptr;
}

void LayoutControl::OnMouseDown(int x, int y) {
    if (auto c = FindChildAt(x, y)) {
        for (const auto &child : children) {
            if (child.get() == c) {
                pressedChild = child;
                c->OnMouseDown(x, y);
                break;
            }
        }
    }
}

void LayoutControl::OnMouseUp(int x, int y) {
    auto pressed = pressedChild.lock();
    pressedChild.reset();

    if (pressed) {
        if (pressed->Contains(x, y)) {
            pressed->OnMouseUp(x, y);
        } else {
            pressed->OnMouseReset();
        }
    }
}

void LayoutControl::OnMouseReset() {
    auto pressed = pressedChild.lock();
    if (pressed) {
        pressed->OnMouseReset();
        pressedChild.reset();
    }
}

void LayoutControl::OnMouseEnter(int x, int y) {}

void LayoutControl::OnMouseLeave(int x, int y) {
    auto hovered = hoveredChild.lock();
    if (hovered) {
        hovered->OnMouseLeave(x, y);
        hoveredChild.reset();
    }
}

void LayoutControl::OnMouseWheel(float deltaX, float deltaY) {}

void LayoutControl::OnKeyDown(int key, int mods) {
    for (auto &c : children) {
        if (c && c->IsEnabled())
            c->OnKeyDown(key, mods);
    }
}

void LayoutControl::OnKeyUp(int key, int mods) {
    for (auto &c : children) {
        if (c && c->IsEnabled())
            c->OnKeyUp(key, mods);
    }
}

void LayoutControl::OnWindowResize(int width, int height) {
    for (auto &c : children) {
        if (c)
            c->OnWindowResize(width, height);
    }
    UpdateLayout();
}

StackPanel::StackPanel(Orientation o) : orientation(o) {}

void StackPanel::SetOrientation(Orientation o) {
    orientation = o;
    UpdateLayout();
}

void StackPanel::SetSpacing(int s) {
    spacing = s;
    UpdateLayout();
}

void StackPanel::SetAlignment(Alignment align) {
    alignment = align;
    UpdateLayout();
}

void StackPanel::UpdateLayout() {
    int currentX = bounds.x;
    int currentY = bounds.y;
    int maxW = 0;
    int maxH = 0;

    for (auto &c : children) {
        if (!c || !c->IsVisible())
            continue;

        c->SetPosition(currentX, currentY);

        if (orientation == Orientation::Vertical) {
            currentY += c->GetHeight() + spacing;
            maxW = std::max(maxW, c->GetWidth());
        } else {
            currentX += c->GetWidth() + spacing;
            maxH = std::max(maxH, c->GetHeight());
        }

        if (orientation == Orientation::Vertical) {
            if (alignment == Alignment::Center) {
                int centerOffset = (bounds.w - c->GetWidth()) / 2;
                c->SetX(bounds.x + centerOffset);
            } else if (alignment == Alignment::End) {
                c->SetX(bounds.x + bounds.w - c->GetWidth());
            } else if (alignment == Alignment::Stretch) {
                c->SetWidth(bounds.w);
                c->SetX(bounds.x);
            }
        }
    }

    if (orientation == Orientation::Vertical) {
        int contentH = currentY - bounds.y;
        if (!children.empty())
            contentH -= spacing;
        if (contentH < 0)
            contentH = 0;
        SetSize(maxW, contentH);
    } else {
        int contentW = currentX - bounds.x;
        if (!children.empty())
            contentW -= spacing;
        if (contentW < 0)
            contentW = 0;
        SetSize(contentW, maxH);
    }
}

void StackPanel::OnWindowResize(int width, int height) {
    LayoutControl::OnWindowResize(width, height);
}
