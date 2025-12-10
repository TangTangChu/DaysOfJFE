#pragma once
#include "Controls.h"
#include <functional>
#include <string>
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"

/**
 * @file Button.h
 * @brief 按钮控件类 - 支持圆角和悬停效果
 */

/**
 * @struct CornerRadius
 * @brief 四角圆角半径配置
 */
struct CornerRadius {
    int A;  // 左上
    int B;  // 右上
    int C;  // 右下
    int D;  // 左下
};

/**
 * Button - 按钮基类
 * 
 * 迁移说明:
 * - wstring → string (UTF-8)
 * - Gdiplus::Color → SkColor
 * - HDC → SkCanvas*
 */
class Button : public Controls {
protected:
    std::string text;
    SkColor textColor;
    CornerRadius radius{20, 20, 20, 20};
    
    Button(const std::string& text, std::function<void()> onClick, SkColor textColor)
        : text(text), textColor(textColor) {
        this->onClick = onClick;
        fontStyle = 1;
    }

public:
    virtual void Draw(SkCanvas* canvas) = 0;
    void SetText(const std::string& newText);
    virtual ~Button() = default;
};

/**
 * PrimaryButton - 主要按钮（带背景的圆角矩形按钮）
 */
class PrimaryButton : public Button {
private:
    SkColor btnColor;
    SkColor hoverBgColor = SkColorSetARGB(127, 255, 255, 0);  // 半透明黄色
    SkColor finalBrush;
    int cornerRadius = 20;

public:
    PrimaryButton(const std::string& text,
                  std::function<void()> onClick,
                  SkColor textColor = SK_ColorWHITE,
                  SkColor backgroundColor = SkColorSetRGB(0, 191, 255))
        : Button(text, onClick, textColor)
        , btnColor(backgroundColor)
        , finalBrush(backgroundColor) {
        
        // 鼠标按下时改变颜色
        this->OnMouseDownHook = [this]() {
            finalBrush = hoverBgColor;
        };
        
        // 鼠标抬起前恢复颜色
        this->OnMouseUpPreHook = [this]() {
            finalBrush = btnColor;
        };
        
        // 鼠标重置时恢复颜色
        this->OnMouseResetHook = [this]() {
            finalBrush = btnColor;
        };
    }
    
    void setBackgroundColor(SkColor color);
    void setHoverBgColor(SkColor color);
    void SetCornerRadius(int r);
    void SetCornerRadius(CornerRadius rad);
    void Draw(SkCanvas* canvas) override;
};

/**
 * TextButton - 文本按钮（只有文字，没有背景）
 */
class TextButton : public Button {
public:
    TextButton(const std::string& text,
               std::function<void()> onClick,
               SkColor textColor = SkColorSetRGB(0, 153, 153))
        : Button(text, onClick, textColor) {
    }
    
    void Draw(SkCanvas* canvas) override;
};
