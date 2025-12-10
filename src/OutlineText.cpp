/**
 * @file OutlineText.cpp
 * @brief 描边文本渲染组件 - 用于对话文本，带救边效果
 * 
 * 与TextBlock相比，增加了:
 * - 双层绘制: 外描边 + 内文本
 * - 自定义颜色: 可设置描边颜色和文本颜色
 * - 相同的优化: UTF-8支持、零拷贝、无锁、字体自动切换
 */
#include "OutlineText.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkPaint.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkFontMgr.h"
#include "include/ports/SkFontMgr_directory.h"
#include <chrono>

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

// 判断字符是否为中文
static bool IsChinese(const std::string& text) {
    for (unsigned char c : text) {
        if (c >= 0xE4 && c <= 0xE9) {
            return true;
        }
    }
    return false;
}

// UTF-8辅助函数：获取前N个字符对应的字节索引
static size_t GetByteIndexForCharCount(const std::string& text, int charCount) {
    if (charCount <= 0) return 0;
    
    size_t byteIndex = 0;
    int currentCharCount = 0;
    
    while (byteIndex < text.length() && currentCharCount < charCount) {
        unsigned char c = text[byteIndex];
        
        if ((c & 0x80) == 0) {
            byteIndex += 1;
        } else if ((c & 0xE0) == 0xC0) {
            byteIndex += 2;
        } else if ((c & 0xF0) == 0xE0) {
            byteIndex += 3;
        } else if ((c & 0xF8) == 0xF0) {
            byteIndex += 4;
        } else {
            byteIndex += 1;
        }
        
        currentCharCount++;
    }
    
    return std::min(byteIndex, text.length());
}

void OutlineText::SetText(const std::string& newText) {
    text = newText;
}

bool OutlineText::IsAnimating() const {
    return m_animating;
}

void OutlineText::Draw(SkCanvas* canvas) {
    if (!IsVisible() || text.empty()) return;

    canvas->save();

    // 计算应该显示的字节长度
    size_t displayByteLength = text.length();
    if (m_animating) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_animStartTime).count();
        
        // 每秒20个字符
        const int charsPerSecond = 20;
        int visibleChars = static_cast<int>((elapsed / 1000.0) * charsPerSecond);
        
        if (visibleChars >= m_totalChars) {
            visibleChars = m_totalChars;
            m_animating = false;
        }
        
        displayByteLength = GetByteIndexForCharCount(text, visibleChars);
    }

    if (displayByteLength == 0) {
        canvas->restore();
        return;
    }

    // 根据文本内容选择字体
    sk_sp<SkTypeface> typeface = IsChinese(text) ? GetChineseTypeface() : GetJapaneseTypeface();
    SkFont font(typeface, fontSize);

    // 绘制描边（外边框）
    if (outlineWidth > 0) {
        SkPaint outlinePaint;
        outlinePaint.setColor(outlineColor);
        outlinePaint.setStyle(SkPaint::kStroke_Style);
        outlinePaint.setStrokeWidth(outlineWidth * 2.0f);
        outlinePaint.setAntiAlias(true);

        canvas->drawSimpleText(text.c_str(), displayByteLength, SkTextEncoding::kUTF8,
                              X, Y, font, outlinePaint);
    }

    // 绘制文本（填充）
    SkPaint textPaint;
    textPaint.setColor(textColor);
    textPaint.setAntiAlias(true);

    canvas->drawSimpleText(text.c_str(), displayByteLength, SkTextEncoding::kUTF8,
                          X, Y, font, textPaint);
    
    canvas->restore();
}

void OutlineText::StartAnimation(bool loop) {
    m_loopAnimation = loop;
    
    // 计算UTF-8字符总数
    m_totalChars = 0;
    size_t i = 0;
    while (i < text.length()) {
        unsigned char c = text[i];
        if ((c & 0x80) == 0) {
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            i += 4;
        } else {
            i += 1;
        }
        m_totalChars++;
    }
    
    m_animStartTime = std::chrono::steady_clock::now();
    m_animating = true;
}

void OutlineText::StopAnimation() {
    m_animating = false;
}

void OutlineText::ShowFullText() {
    m_animating = false;
}

void OutlineText::ClearText() {
    m_animating = false;
}
