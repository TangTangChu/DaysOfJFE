#include "Button.h"
#include "SkFont.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkRRect.h"
#include "include/core/SkFontMgr.h"
#include "include/ports/SkFontMgr_directory.h"

// 全局字体管理器和字体单例
static sk_sp<SkFontMgr> GetGlobalFontMgr() {
    static sk_sp<SkFontMgr> fontMgr = SkFontMgr_New_Custom_Directory("assets/font");
    return fontMgr;
}

static sk_sp<SkTypeface> GetChineseTypeface() {
    static sk_sp<SkTypeface> typeface = GetGlobalFontMgr()->makeFromFile("assets/font/SourceHanSerifSC.otf");
    return typeface;
}

static sk_sp<SkTypeface> GetJapaneseTypeface() {
    static sk_sp<SkTypeface> typeface = GetGlobalFontMgr()->makeFromFile("assets/font/TsukushiMincho.otf");
    return typeface;
}

static bool IsChinese(const std::string& text) {
    for (unsigned char c : text) {
        if (c >= 0xE4 && c <= 0xE9) {
            return true;
        }
    }
    return false;
}

void Button::SetText(const std::string& newText) {
    text = newText;
}

void PrimaryButton::setBackgroundColor(SkColor color) {
    btnColor = color;
    finalBrush = color;
}

void PrimaryButton::setHoverBgColor(SkColor color) {
    hoverBgColor = color;
}

void PrimaryButton::SetCornerRadius(CornerRadius rad) {
    this->radius = rad;
}

void PrimaryButton::SetCornerRadius(int r) {
    this->radius = CornerRadius{r, r, r, r};
}

void PrimaryButton::Draw(SkCanvas* canvas) {
    if (!IsVisible()) return;
    
    // 创建圆角矩形
    SkRect bounds = SkRect::MakeXYWH(X, Y, Width, Height);
    
    // 设置每个角的半径
    SkVector radii[4] = {
        {SkIntToScalar(radius.A), SkIntToScalar(radius.A)},  // 左上
        {SkIntToScalar(radius.B), SkIntToScalar(radius.B)},  // 右上
        {SkIntToScalar(radius.C), SkIntToScalar(radius.C)},  // 右下
        {SkIntToScalar(radius.D), SkIntToScalar(radius.D)}   // 左下
    };
    
    SkRRect rrect;
    rrect.setRectRadii(bounds, radii);
    
    // 绘制背景
    SkPaint bgPaint;
    bgPaint.setColor(finalBrush);
    bgPaint.setAntiAlias(true);
    bgPaint.setStyle(SkPaint::kFill_Style);
    canvas->drawRRect(rrect, bgPaint);
    
    // 绘制文本
    SkPaint textPaint;
    textPaint.setColor(SK_ColorWHITE);
    textPaint.setAntiAlias(true);
    
    // 根据文本内容选择字体
    sk_sp<SkTypeface> typeface = IsChinese(text) ? GetChineseTypeface() : GetJapaneseTypeface();
    SkFont font(typeface, fontSize);
    if (fontStyle == 1) {
        font.setEmbolden(true);
    }
    
    // 测量文本尺寸以居中
    SkRect textBounds;
    font.measureText(text.c_str(), text.size(), SkTextEncoding::kUTF8, &textBounds);
    
    // 计算居中位置
    float textX = X + (Width - textBounds.width()) / 2 - textBounds.x();
    float textY = Y + (Height - textBounds.height()) / 2 - textBounds.y();
    
    canvas->drawString(text.c_str(), textX, textY, font, textPaint);
}

void TextButton::Draw(SkCanvas* canvas) {
    if (!IsVisible()) return;
    
    // 只绘制文本，没有背景
    SkPaint textPaint;
    textPaint.setColor(textColor);
    textPaint.setAntiAlias(true);
    
    // 根据文本内容选择字体
    sk_sp<SkTypeface> typeface = IsChinese(text) ? GetChineseTypeface() : GetJapaneseTypeface();
    SkFont font(typeface, fontSize);
    if (fontStyle == 1) {
        font.setEmbolden(true);
    }
    
    // 测量文本尺寸以居中
    SkRect textBounds;
    font.measureText(text.c_str(), text.size(), SkTextEncoding::kUTF8, &textBounds);
    
    // 计算居中位置
    float textX = X + (Width - textBounds.width()) / 2 - textBounds.x();
    float textY = Y + (Height - textBounds.height()) / 2 - textBounds.y();
    
    canvas->drawString(text.c_str(), textX, textY, font, textPaint);
}
