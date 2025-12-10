#pragma once
#include "Controls.h"
#include <string>
#include <atomic>
#include <chrono>
#include "include/core/SkColor.h"


class OutlineText : public Controls {
private:
    int fontSize;
    int outlineWidth;
    std::string text;
    SkColor outlineColor;
    SkColor textColor;
    std::atomic<bool> m_animating{false};
    std::chrono::steady_clock::time_point m_animStartTime;
    int m_totalChars = 0;
    bool m_loopAnimation = false;
    std::string fontFamilyName = "WenQuanYi Micro Hei";

public:
    OutlineText(const std::string& text, 
                int fontsize = 14, 
                int thick = 1,
                SkColor outlineColor = SkColorSetARGB(255, 25, 25, 25),
                SkColor textColor = SkColorSetARGB(255, 255, 255, 255))
        : text(text)
        , fontSize(fontsize)
        , outlineWidth(thick)
        , outlineColor(outlineColor)
        , textColor(textColor) {
    }
    
    ~OutlineText() = default;

    void SetText(const std::string& newText);
    bool IsAnimating() const;
    void Draw(SkCanvas* canvas) override;

    void StartAnimation(bool loop = false);
    void StopAnimation();
    void ShowFullText();
    void ClearText();
};
