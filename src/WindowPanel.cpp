/**
 * @file WindowPanel.cpp
 * @brief 窗口面板基类 - 所有游戏窗口的基础
 * 
 * 三层渲染架构:
 * 1. Background层: 背景图片
 * 2. Controls层: 角色立绘
 * 3. Foreground层: 对话框、按钮、UI元素
 * 
 * 特性:
 * - 背景透明度控制
 * - 控件管理和事件分发
 * - 对话框显社/隐藏
 */
#include "WindowPanel.h"
#include "Controls.h"
#include "IGlobalEvent.h"
#include "include/core/SkBitmap.h"
#include <iostream>
#include <algorithm>

void WindowPanel::AddBackgroundControl(std::shared_ptr<Controls> ctrl) {
    backgroundControls.push_back(ctrl);
}

void WindowPanel::AddControl(std::shared_ptr<Controls> ctrl) {
    controls.push_back(ctrl);
}

void WindowPanel::AddMidgroundControl(std::shared_ptr<Controls> ctrl) {
    controls.push_back(ctrl);
}

void WindowPanel::AddForegroundControl(std::shared_ptr<Controls> ctrl) {
    foregroundControls.push_back(ctrl);
}

bool WindowPanel::RemoveControl(std::shared_ptr<Controls> ctrl) {
    bool removed = false;
    
    auto removeFrom = [&](std::vector<std::shared_ptr<Controls>>& vec) {
        auto it = std::remove(vec.begin(), vec.end(), ctrl);
        if (it != vec.end()) {
            vec.erase(it, vec.end());
            removed = true;
        }
    };
    
    removeFrom(backgroundControls);
    removeFrom(controls);
    removeFrom(foregroundControls);
    
    return removed;
}

void WindowPanel::ClearAllControls() {
    backgroundControls.clear();
    controls.clear();
    foregroundControls.clear();
}

void WindowPanel::SetBackground(sk_sp<SkImage> img) {
    background = img;
}

void WindowPanel::SetBackgroundImage(const std::string& path) {
    sk_sp<SkData> data = SkData::MakeFromFileName(path.c_str());
    if (data) {
        std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(data);
        if (codec) {
            SkImageInfo info = codec->getInfo();
            SkBitmap bitmap;
            bitmap.allocPixels(info);
            if (codec->getPixels(info, bitmap.getPixels(), bitmap.rowBytes()) == SkCodec::kSuccess) {
                background = SkImages::RasterFromBitmap(bitmap);
            }
        }
    }
}

void WindowPanel::ClearBackground() {
    background.reset();
}

std::vector<std::shared_ptr<Controls>> WindowPanel::GetAllControls() const {
    std::vector<std::shared_ptr<Controls>> allControls;
    
    // 按层级顺序合并
    allControls.insert(allControls.end(), backgroundControls.begin(), backgroundControls.end());
    allControls.insert(allControls.end(), controls.begin(), controls.end());
    allControls.insert(allControls.end(), foregroundControls.begin(), foregroundControls.end());
    
    return allControls;
}

Controls* WindowPanel::FindTopControlAt(int x, int y) {
    auto allControls = GetAllControls();
    
    for (auto it = allControls.rbegin(); it != allControls.rend(); ++it) {
        if ((*it)->IsVisible() && (*it)->IsPointInside(x, y)) {
            return it->get();
        }
    }
    
    return nullptr;
}

void WindowPanel::Draw(SkCanvas* canvas) {
    static int frameCount = 0;
    frameCount++;
    
    // 绘制背景
    if (background) {
        SkPaint bgPaint;
        bgPaint.setAlpha(BgAlpha);
        // 缩放背景以填充整个窗口
        SkRect dest = SkRect::MakeWH(canvas->imageInfo().width(), canvas->imageInfo().height());
        canvas->drawImageRect(background, dest, SkSamplingOptions(), &bgPaint);
    }
    
    // 绘制所有控件
    auto allControls = GetAllControls();
    if (frameCount < 10) {
        std::cout << "[WindowPanel::Draw] 帧=" << frameCount 
                  << " 控件总数=" << allControls.size()
                  << " (bg=" << backgroundControls.size() 
                  << " mid=" << controls.size()
                  << " fg=" << foregroundControls.size() << ")" << std::endl;
    }
    
    for (const auto& ctrl : allControls) {
        if (ctrl && ctrl->IsVisible()) {
            ctrl->Draw(canvas);
        }
    }
}

void WindowPanel::HandleSDLEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            HandleMouseDown(event.button.x, event.button.y);
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            HandleMouseUp(event.button.x, event.button.y);
        }
        else if (event.button.button == SDL_BUTTON_RIGHT) {
            if (OnMouseRBUPHook) {
                OnMouseRBUPHook();
            }
        }
    }
    else if (event.type == SDL_KEYDOWN) {
        if (OnKeyDownHook) {
            OnKeyDownHook(event.key.keysym.sym);
        }
    }
    else if (event.type == SDL_KEYUP) {
        if (OnKeyUpHook) {
            OnKeyUpHook(event.key.keysym.sym);
        }
    }
    else if (event.type == SDL_MOUSEMOTION) {
        // 鼠标移动事件 - 可用于悬停效果
        // TODO: 实现悬停高亮效果
    }
}

void WindowPanel::HandleMouseDown(int x, int y) {
    Controls* ctrl = FindTopControlAt(x, y);
    if (ctrl) {
        pressedControl = ctrl;
        ctrl->OnMouseDown(x, y);
    }
}

void WindowPanel::HandleMouseUp(int x, int y) {
    if (pressedControl) {
        pressedControl->OnMouseUp(x, y);
        pressedControl = nullptr;
    }
}

void WindowPanel::SetGlobalEvent(IGlobalEvent* ge) {
    m_globalEvent = ge;
}
