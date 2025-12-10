#pragma once
#include "Controls.h"
#include <functional>

/**
 * @file ClickPaper.h
 * @brief 透明点击区域 - 不可见但可交互的控件
 */
class ClickPaper : public Controls {
public:
    ClickPaper(std::function<void()> onClick) {
        this->OnMouseDownHook = onClick;
    }
    
    void Draw(SkCanvas* canvas) override {}
    
    void OnMouseUp(int x, int y) override;
};
