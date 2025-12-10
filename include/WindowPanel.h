#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <SDL2/SDL.h>
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
#include "include/codec/SkCodec.h"

// 前置声明
class Controls;
class IGlobalEvent;

/**
 * WindowPanel - 窗口面板基类
 * 
 * 迁移说明:
 * - Draw(HDC) → Draw(SkCanvas*)
 * - HandleExMessage(ExMessage) → HandleSDLEvent(SDL_Event)
 * - Gdiplus::Image → sk_sp<SkImage>
 */
class WindowPanel {
protected:
    std::vector<std::shared_ptr<Controls>> backgroundControls;
    std::vector<std::shared_ptr<Controls>> controls;
    std::vector<std::shared_ptr<Controls>> foregroundControls;
    
    // 事件回调
    std::function<void()> OnMouseRBUPHook;
    std::function<void(SDL_Keycode)> OnKeyDownHook;
    std::function<void(SDL_Keycode)> OnKeyUpHook;
    
    IGlobalEvent* m_globalEvent = nullptr;
    
public:
    // 背景图片及透明度
    sk_sp<SkImage> background;  // 原版: std::unique_ptr<Gdiplus::Image>
    uint8_t BgAlpha = 255;
    Controls* pressedControl = nullptr;
    
    virtual ~WindowPanel() = default;

    // 控件管理
    virtual void AddBackgroundControl(std::shared_ptr<Controls> ctrl);
    virtual void AddControl(std::shared_ptr<Controls> ctrl);
    virtual void AddMidgroundControl(std::shared_ptr<Controls> ctrl);
    virtual void AddForegroundControl(std::shared_ptr<Controls> ctrl);
    virtual bool RemoveControl(std::shared_ptr<Controls> ctrl);
    virtual void ClearAllControls();

    // 背景管理
    virtual void SetBackground(sk_sp<SkImage> img);
    virtual void SetBackgroundImage(const std::string& path);
    virtual void ClearBackground();
    virtual sk_sp<SkImage> GetBackground() const { return background; }

    // 获取所有控件（用于绘制）
    virtual std::vector<std::shared_ptr<Controls>> GetAllControls() const;
    
    // 查找控件
    virtual Controls* FindTopControlAt(int x, int y);

    // 绘制
    virtual void Draw(SkCanvas* canvas);
    
    // 窗口生命周期回调
    virtual void OnShow() {}  // 窗口显示时调用

    // 事件处理 - ExMessage → SDL_Event
    virtual void HandleSDLEvent(const SDL_Event& event);
    virtual void HandleMouseDown(int x, int y);
    virtual void HandleMouseUp(int x, int y);
    
    // 全局事件
    virtual void SetGlobalEvent(IGlobalEvent* globalEvent);
};
