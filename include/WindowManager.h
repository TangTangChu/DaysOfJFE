#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <SDL2/SDL.h>
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
// 前置声明 - 这些类稍后需要迁移
class Controls;
class WindowPanel;
class IGlobalEvent;

/**
 * WindowManager - 窗口管理器
 * 
 * 迁移说明:
 * - HDC → SkCanvas*
 * - ExMessage → SDL_Event
 * - Gdiplus::Image → sk_sp<SkImage>
 * - wstring → string (UTF-8)
 */
class WindowManager {
private:
    std::vector<std::shared_ptr<WindowPanel>> windows;
    std::shared_ptr<WindowPanel> currentWindow;
    std::shared_ptr<WindowPanel> pendingWindow;  // 待切换的窗口
    int pendingWindowIndex = -1;  // 待切换的窗口索引
    SkCanvas* canvas;  // 原版: HDC hdc
    IGlobalEvent* m_globalEvent;  // 改为指针，延迟初始化
    
    enum Layer { 
        BACKGROUND, 
        MIDGROUND, 
        FOREGROUND 
    };
    
    bool isShowDialog;

public:
    // 构造函数 - HDC 改为 SkCanvas*
    explicit WindowManager(SkCanvas* canvas);
    ~WindowManager();

    // 窗口管理
    void AddWindow(std::shared_ptr<WindowPanel> window);
    void SwitchWindow(int index);
    void SwitchWindow(std::shared_ptr<WindowPanel> window);
    std::shared_ptr<WindowPanel> GetWindow(int index);

    // 控件管理
    void AddBackgroundControl(std::shared_ptr<Controls> ctrl);
    void AddMidgroundControl(std::shared_ptr<Controls> ctrl);
    void AddForegroundControl(std::shared_ptr<Controls> ctrl);
    bool AddControl(std::shared_ptr<Controls> ctrl, int layer = MIDGROUND);
    void RemoveControl(std::shared_ptr<Controls> ctrl);
    void ClearAllControls();

    // 背景管理 - Image → sk_sp<SkImage>
    void SetBackground(sk_sp<SkImage> img);
    void ClearBackground();

    // 渲染
    void Redraw();
    
    // 处理待切换的窗口
    void ProcessPendingWindowSwitch();

    // 事件分发 - ExMessage → SDL_Event
    void DispatchEvent(const SDL_Event& event);

    // 对话框
    void ShowDialog(std::string text, std::function<void()> yes, std::function<void()> no);
    void CloseDialog();
    
    // 获取 canvas（某些情况下需要直接访问）
    SkCanvas* GetCanvas() const { return canvas; }
};
