#pragma once
#include <functional>
#include <SDL2/SDL.h>
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"

/**
 * @file Controls.h
 * @brief 控件基类 - 所有UI控件的抽象基类
 * 
 * 迁移说明:
 * - Draw(HDC) → Draw(SkCanvas*)
 * - Gdiplus::Rect → SkRect
 * - ExMessage → SDL_Event
 * - Color → SkColor
 */
class Controls {
protected:
    int X = 0, Y = 0, Width = 100, Height = 40;
    int fontStyle = 0, fontSize = 12;
    bool visible = true;
    bool enabled = true;
    SkRect rect;
    
    // 回调函数
    std::function<void()> onClick;
    std::function<void()> OnMouseUpPreHook;
    std::function<void()> OnMouseUpAftHook;
    std::function<void()> OnMouseDownHook;
    std::function<void()> OnMouseResetHook;
    std::function<void()> OnSwitch;
    
public:
    virtual ~Controls() = default;

    // 绘制 - HDC 改为 SkCanvas*
    virtual void Draw(SkCanvas* canvas) = 0;

    // 事件处理
    virtual void OnMouseUp(int x, int y);
    virtual void OnMouseDown(int x, int y);
    virtual void OnMouseReset();
    virtual void HandleEvent(const SDL_Event& event);

    // 位置和尺寸
    SkRect GetBounds() const;
    bool Contains(int x, int y) const;
    bool IsPointInside(int x, int y) const;  // 别名，与 Contains 相同
    
    void SetPosition(int x, int y);
    void SetSize(int width, int height);
    void SetRect(SkRect rect);
    void SetFontSize(int fontsize);
    void SetFontStyle(int fontstyle);
    
    // 可见性和启用状态
    void SetVisible(bool v);
    bool IsVisible() const;
    void SetEnabled(bool e);
    bool IsEnabled() const;
    
    // 设置回调
    void SetOnClick(std::function<void()> onClick);
    void SetOnMouseUpPreHook(std::function<void()> OnMouseUpPreHook);
    void SetOnMouseUpAftHook(std::function<void()> OnMouseUpAftHook);
    void SetOnMouseDownHook(std::function<void()> OnMouseDownHook);
    void SetOnMouseResetHook(std::function<void()> OnMouseResetHook);
};
