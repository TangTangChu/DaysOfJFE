/**
 * @file WindowManager.cpp
 * @brief 窗口管理器 - 管理多个游戏窗口的切换和渲染
 * 
 * 功能:
 * - 窗口注册和切换
 * - 事件分发到当前活动窗口
 * - 全局事件接口(IGlobalEvent)
 * - 统一的绘制调度
 */
#include "WindowManager.h"
#include "WindowPanel.h"
#include "IGlobalEvent.h"
#include "Controls.h"
#include <algorithm>
#include <iostream>

// 构造函数 - HDC 改为 SkCanvas*
WindowManager::WindowManager(SkCanvas* canvas) 
    : canvas(canvas)
    , pendingWindowIndex(-1)
    , isShowDialog(false) 
{
    // 创建全局事件对象
    m_globalEvent = new IGlobalEvent(this);
}

WindowManager::~WindowManager() {
    if (m_globalEvent) {
        delete m_globalEvent;
        m_globalEvent = nullptr;
    }
}

void WindowManager::AddWindow(std::shared_ptr<WindowPanel> window) {
    if (m_globalEvent) {
        window->SetGlobalEvent(m_globalEvent);
    }
    windows.push_back(window);
}

std::shared_ptr<WindowPanel> WindowManager::GetWindow(int index) {
    if (index >= 0 && index < windows.size()) {
        return windows[index];
    }
    return nullptr;
}

void WindowManager::SwitchWindow(int index) {
    std::cout << "请求切换到窗口 " << index << std::endl;
    if (index >= 0 && index < windows.size()) {
        pendingWindowIndex = index;
        pendingWindow.reset();  // 清空 pendingWindow
    }
}

void WindowManager::SwitchWindow(std::shared_ptr<WindowPanel> window) {
    std::cout << "请求切换到新窗口" << std::endl;
    pendingWindow = window;
    pendingWindowIndex = -1;  // 清空索引
}

void WindowManager::ProcessPendingWindowSwitch() {
    if (pendingWindowIndex >= 0) {
        std::cout << "执行窗口切换: " << pendingWindowIndex << std::endl;
        if (pendingWindowIndex < windows.size()) {
            currentWindow = windows[pendingWindowIndex];
            if (currentWindow) {
                currentWindow->OnShow();  // 调用窗口显示回调
            }
        }
        pendingWindowIndex = -1;
    } else if (pendingWindow) {
        std::cout << "执行新窗口切换" << std::endl;
        currentWindow = pendingWindow;
        if (currentWindow) {
            currentWindow->OnShow();  // 调用窗口显示回调
        }
        pendingWindow.reset();
    }
}

void WindowManager::AddBackgroundControl(std::shared_ptr<Controls> ctrl) {
    if (currentWindow) {
        currentWindow->AddBackgroundControl(ctrl);
    }
}

void WindowManager::AddMidgroundControl(std::shared_ptr<Controls> ctrl) {
    if (currentWindow) {
        currentWindow->AddMidgroundControl(ctrl);
    }
}

void WindowManager::AddForegroundControl(std::shared_ptr<Controls> ctrl) {
    if (currentWindow) {
        currentWindow->AddForegroundControl(ctrl);
    }
}

bool WindowManager::AddControl(std::shared_ptr<Controls> ctrl, int layer) {
    if (!currentWindow) {
        return false;
    }

    switch (layer) {
        case BACKGROUND:
            currentWindow->AddBackgroundControl(ctrl);
            break;
        case MIDGROUND:
            currentWindow->AddControl(ctrl);
            break;
        case FOREGROUND:
            currentWindow->AddForegroundControl(ctrl);
            break;
        default:
            return false;
    }
    
    return true;
}

void WindowManager::RemoveControl(std::shared_ptr<Controls> ctrl) {
    if (currentWindow) {
        currentWindow->RemoveControl(ctrl);
    }
}

void WindowManager::ClearAllControls() {
    if (currentWindow) {
        currentWindow->ClearAllControls();
    }
}

void WindowManager::SetBackground(sk_sp<SkImage> img) {
    if (currentWindow) {
        currentWindow->SetBackground(img);
    }
}

void WindowManager::ClearBackground() {
    if (currentWindow) {
        currentWindow->ClearBackground();
    }
}

void WindowManager::Redraw() {
    static int frameCount = 0;
    frameCount++;
    
    if (!currentWindow) {
        return;
    }

    // 绘制背景图片 (如果存在)
    auto bg = currentWindow->GetBackground();
    if (bg) {
        // 获取背景透明度
        float alpha = std::clamp(currentWindow->BgAlpha / 255.0f, 0.0f, 1.0f);

        // 目标矩形 (全屏)
        SkRect destRect = SkRect::MakeWH(1600, 900);
        
        // 源图片尺寸
        float imageWidth = static_cast<float>(bg->width());
        float imageHeight = static_cast<float>(bg->height());
        float imageRatio = imageWidth / imageHeight;
        float destRatio = destRect.width() / destRect.height();

        // 计算裁剪区域 (保持宽高比，居中裁剪)
        SkRect srcRect = SkRect::MakeWH(imageWidth, imageHeight);
        if (imageRatio > destRatio) {
            // 图片更宽，裁剪左右
            float requiredWidth = imageHeight * destRatio;
            srcRect.fLeft = (imageWidth - requiredWidth) / 2;
            srcRect.fRight = srcRect.fLeft + requiredWidth;
        } else {
            // 图片更高，裁剪上下
            float requiredHeight = imageWidth / destRatio;
            srcRect.fTop = (imageHeight - requiredHeight) / 2;
            srcRect.fBottom = srcRect.fTop + requiredHeight;
        }

        // 绘制背景图片（带透明度）
        SkPaint paint;
        paint.setAlpha(static_cast<U8CPU>(currentWindow->BgAlpha));
        paint.setAntiAlias(true);
        
        canvas->drawImageRect(
            bg,
            srcRect,
            destRect,
            SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kLinear),
            &paint,
            SkCanvas::kStrict_SrcRectConstraint
        );
    }

    // 绘制所有控件
    auto allControls = currentWindow->GetAllControls();
    
    for (auto& control : allControls) {
        if (control && control->IsVisible()) {
            control->Draw(canvas);
        }
    }
}

// 事件分发 - ExMessage → SDL_Event
void WindowManager::DispatchEvent(const SDL_Event& event) {
    if (!currentWindow) {
        return;
    }
    
    currentWindow->HandleSDLEvent(event);  // 需要在 WindowPanel 中实现
}

void WindowManager::ShowDialog(std::string text, std::function<void()> yes, std::function<void()> no) {
    if (!isShowDialog) {
        // TODO: 实现对话框显示
        std::cout << "显示对话框: " << text << std::endl;
        isShowDialog = true;
    }
}

void WindowManager::CloseDialog() {
    if (isShowDialog) {
        isShowDialog = false;
    }
}
