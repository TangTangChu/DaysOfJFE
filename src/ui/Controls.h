#pragma once
#include "gfx/IRenderer.h"
#include "gfx/geom/Geom.h"
#include <functional>

class Controls {
  protected:
    gfx::Rect bounds;
    int fontStyle = 0;
    int fontSize = 12;
    bool visible = true;
    bool enabled = true;
    bool hovered = false;
    bool pressed = false;

    std::function<void()> onClick;
    std::function<void()> onMouseUpPreHook;
    std::function<void()> onMouseUpAftHook;
    std::function<void()> onMouseDownHook;
    std::function<void()> onMouseResetHook;
    std::function<void()> onMouseEnterHook;
    std::function<void()> onMouseLeaveHook;
    std::function<void(float, float)> onMouseWheelHook;
    std::function<void(int, int)> onKeyDownHook;
    std::function<void(int, int)> onKeyUpHook;

  public:
    Controls() = default;
    virtual ~Controls() = default;

    virtual void Draw(IRenderer &r) = 0;

    virtual void OnMouseDown(int x, int y);
    virtual void OnMouseUp(int x, int y);
    virtual void OnMouseReset();
    virtual void OnMouseEnter(int x, int y);
    virtual void OnMouseLeave(int x, int y);
    virtual void OnMouseWheel(float deltaX, float deltaY);
    virtual void OnKeyDown(int key, int mods);
    virtual void OnKeyUp(int key, int mods);

    virtual void OnWindowResize(int width, int height) {}

    bool Contains(int x, int y) const;

    void SetBounds(const gfx::Rect &r) { bounds = r; }

    const gfx::Rect &GetBounds() const { return bounds; }
    int GetX() const;
    int GetY() const;
    int GetWidth() const;
    int GetHeight() const;

    void SetX(int x);
    void SetY(int y);
    void SetWidth(int w);
    void SetHeight(int h);

    void SetPosition(int x, int y);
    void SetSize(int w, int h);
    void SetBounds(int x, int y, int w, int h);

    int GetFontSize() const;
    void SetFontSize(int fontsize);

    int GetFontStyle() const;
    void SetFontStyle(int fontstyle);

    void SetVisible(bool v);
    bool IsVisible() const;

    void SetEnabled(bool e);
    bool IsEnabled() const;

    bool IsHovered() const { return hovered; }
    bool IsPressed() const { return pressed; }

    void SetOnClick(std::function<void()> click);
    void SetOnMouseUpPreHook(std::function<void()> hook);
    void SetOnMouseUpAftHook(std::function<void()> hook);
    void SetOnMouseDownHook(std::function<void()> hook);
    void SetOnMouseResetHook(std::function<void()> hook);
    void SetOnMouseEnterHook(std::function<void()> hook);
    void SetOnMouseLeaveHook(std::function<void()> hook);
    void SetOnMouseWheelHook(std::function<void(float, float)> hook);
    void SetOnKeyDownHook(std::function<void(int, int)> hook);
    void SetOnKeyUpHook(std::function<void(int, int)> hook);
};
