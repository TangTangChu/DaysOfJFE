#pragma once
#include "Controls.h"
#include <string>

class TitleTransition : public Controls {
private:
    std::string text;
    int alpha = 255;  // 透明度，用于淡入淡出效果

public:
    TitleTransition(const std::string& text = "标题");
    void SetText(const std::string& text);
    void SetAlpha(int alpha);
    void Draw(SkCanvas* canvas) override;
};
