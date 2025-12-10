#pragma once
#include "Controls.h"
#include <string>
#include <atomic>
#include <chrono>
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkFont.h"
#include "include/core/SkTypeface.h"

/**
 * TextBlock - 文本显示控件
 * 
 * 迁移说明:
 * - wstring → string (UTF-8)
 * - Gdiplus::Color → SkColor
 * - Gdiplus::FontFamily → SkTypeface
 * - Gdiplus::StringFormat → 手动实现对齐
 * - 保留文本动画功能
 */
class TextBlock : public Controls {
public:
    // 文本对齐方式
    enum class Alignment {
        Left,
        Center,
        Right
    };
    
    enum class VerticalAlignment {
        Top,
        Center,
        Bottom
    };

private:
    std::string text;
    std::string showText;
    SkColor textColor;
    std::string fontFamilyName;
    
    Alignment horizontalAlign = Alignment::Left;
    VerticalAlignment verticalAlign = VerticalAlignment::Top;
    
    // 动画相关 - 基于时间戳的打字机效果
    std::atomic<bool> m_animating{false};
    std::chrono::steady_clock::time_point m_animStartTime;
    int m_totalChars = 0;  // 总字符数（UTF-8字符数）
    bool m_loopAnimation = false;
    
    // 字体缓存（避免每帧重新创建）
    mutable sk_sp<SkTypeface> cachedTypeface;
    mutable SkFont cachedFont;
    mutable bool fontCached = false;

public:
    TextBlock(const std::string& text, 
              int fontsize = 14,
              SkColor textColor = SK_ColorWHITE)
        : text(text)
        , showText(text)
        , textColor(textColor)
        , fontFamilyName("WenQuanYi Micro Hei") {  // 使用系统实际存在的字体
        this->fontSize = fontsize;
    }
    
    ~TextBlock() {
        stopAnimation();
    }

    void Draw(SkCanvas* canvas) override;
    
    // 文本设置
    void SetText(const std::string& newText);
    void SetTextColor(SkColor color) { textColor = color; }
    void SetFontFamily(const std::string& family) { fontFamilyName = family; }
    
    // 对齐方式
    void SetHorizontalAlignment(Alignment align) { horizontalAlign = align; }
    void SetVerticalAlignment(VerticalAlignment align) { verticalAlign = align; }
    
    // 动画控制
    bool IsAnimating() const;
    void startAnimation(bool loop = false);
    void stopAnimation();
    void showFullText();
    void clearText();

private:
    void TextAnimation();
};
