#include "TitleTransition.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkPaint.h"
#include "include/core/SkTypeface.h"
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

TitleTransition::TitleTransition(const std::string& text) : text(text) {
    // 默认居中显示，覆盖整个屏幕（1600x900）
    SetRect(SkRect::MakeWH(1600, 900));
}

void TitleTransition::SetText(const std::string& newText) {
    text = newText;
}

void TitleTransition::SetAlpha(int a) {
    alpha = std::max(0, std::min(255, a));
}

void TitleTransition::Draw(SkCanvas* canvas) {
    if (!IsVisible() || text.empty()) return;

    // 根据文本内容选择字体
    sk_sp<SkTypeface> typeface = IsChinese(text) ? GetChineseTypeface() : GetJapaneseTypeface();
    SkFont font(typeface, 72);  // 大字体
    font.setEmbolden(true);

    // 半透明黑色背景
    SkPaint bgPaint;
    bgPaint.setColor(SkColorSetARGB(alpha * 2 / 3, 0, 0, 0));
    SkRect rect = SkRect::MakeXYWH(X, Y, Width, Height);
    canvas->drawRect(rect, bgPaint);

    // 绘制文字
    SkPaint textPaint;
    textPaint.setColor(SkColorSetARGB(alpha, 255, 255, 255));
    textPaint.setAntiAlias(true);

    // 居中绘制
    SkRect bounds;
    font.measureText(text.c_str(), text.size(), SkTextEncoding::kUTF8, &bounds);
    
    float textX = X + (Width - bounds.width()) / 2 - bounds.x();
    float textY = Y + (Height - bounds.height()) / 2 - bounds.y();

    canvas->drawSimpleText(text.c_str(), text.size(), SkTextEncoding::kUTF8,
                          textX, textY, font, textPaint);
}
