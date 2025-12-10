/**
 * @file TextBlock.cpp
 * @brief 简单文本渲染组件 - 支持UTF-8、打字机动画、自动字体切换
 * 
 * 核心特性:
 * - UTF-8字符边界检测: 正确处理中文字符
 * - 零拷贝绘制: drawSimpleText 避免字符串临时对象
 * - 无锁架构: 使用原子操作而非互斥锁
 * - 与渲染同步: 基于时间戳计算，60 FPS固定帧率
 * - 字体自动切换: 中文使用思源宋体，日文使用筑紫明朝体
 */
#include "TextBlock.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkFontMetrics.h"
#include "include/core/SkTypeface.h"
#include "include/ports/SkFontMgr_directory.h"
#include <chrono>

// ============================================================================
// 全局字体管理 - 单例模式避免重复加载
// ============================================================================

/**
 * @brief 获取全局字体管理器单例
 * @return 字体管理器指针
 */
static sk_sp<SkFontMgr> GetGlobalFontMgr() {
    static sk_sp<SkFontMgr> fontMgr = SkFontMgr_New_Custom_Directory("assets/font");
    return fontMgr;
}

/**
 * @brief 获取中文字体（思源宋体）
 * @return 中文字体typeface
 */
static sk_sp<SkTypeface> GetChineseTypeface() {
    static sk_sp<SkTypeface> typeface = GetGlobalFontMgr()->makeFromFile("assets/font/SourceHanSerifSC.otf");
    return typeface;
}

/**
 * @brief 获取日文字体（筑紫明朝体）
 * @return 日文字体typeface
 */
static sk_sp<SkTypeface> GetJapaneseTypeface() {
    static sk_sp<SkTypeface> typeface = GetGlobalFontMgr()->makeFromFile("assets/font/TsukushiMincho.otf");
    return typeface;
}

// ============================================================================
// UTF-8辅助函数
// ============================================================================

/**
 * @brief 判断文本是否包含中文字符
 * @param text UTF-8编码的文本
 * @return true如果包含中文，否则false
 */
static bool IsChinese(const std::string& text) {
    for (unsigned char c : text) {
        // 中文UTF-8范围: E4-E9 (CJK统一汉字)
        if (c >= 0xE4 && c <= 0xE9) {
            return true;
        }
    }
    return false;
}

/**
 * @brief 获取前N个UTF-8字符对应的字节索引
 * @param text UTF-8编码的文本
 * @param charCount 字符数量（不是字节数）
 * @return 字节索引
 * 
 * 正确处理UTF-8多字节字符：
 * - 1字节: 0xxxxxxx (ASCII)
 * - 2字节: 110xxxxx 10xxxxxx
 * - 3字节: 1110xxxx 10xxxxxx 10xxxxxx (中文)
 * - 4字节: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx (Emoji)
 */
static size_t GetByteIndexForCharCount(const std::string& text, int charCount) {
    if (charCount <= 0) return 0;
    
    size_t byteIndex = 0;
    int currentCharCount = 0;
    
    while (byteIndex < text.length() && currentCharCount < charCount) {
        unsigned char c = text[byteIndex];
        
        // UTF-8字符长度判断
        if ((c & 0x80) == 0) {
            byteIndex += 1;  // ASCII (0xxxxxxx)
        } else if ((c & 0xE0) == 0xC0) {
            byteIndex += 2;  // 2字节 (110xxxxx)
        } else if ((c & 0xF0) == 0xE0) {
            byteIndex += 3;  // 3字节 (1110xxxx) - 大多数中文
        } else if ((c & 0xF8) == 0xF0) {
            byteIndex += 4;  // 4字节 (11110xxx) - Emoji等
        } else {
            // 非法UTF-8，跳过
            byteIndex += 1;
        }
        
        currentCharCount++;
    }
    
    return std::min(byteIndex, text.length());
}

void TextBlock::SetText(const std::string& newText) {
    text = newText;
}

bool TextBlock::IsAnimating() const {
    return m_animating;
}

void TextBlock::Draw(SkCanvas* canvas) {
    if (!IsVisible() || text.empty()) return;
    
    canvas->save();
    
    // 根据文本内容选择字体
    sk_sp<SkTypeface> typeface = IsChinese(text) ? GetChineseTypeface() : GetJapaneseTypeface();
    SkFont font(typeface, fontSize);
    if (fontStyle == 1) {
        font.setEmbolden(true);
    }
    
    // 创建画笔
    SkPaint paint;
    paint.setColor(textColor);
    paint.setAntiAlias(true);
    
    // 计算应该显示的字符数（UTF-8字符数，不是字节数）
    size_t displayByteLength = text.length();
    if (m_animating) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_animStartTime).count();
        
        // 每秒20个字符（UTF-8字符，不是字节）
        const int charsPerSecond = 20;
        int visibleChars = static_cast<int>((elapsed / 1000.0) * charsPerSecond);
        
        if (visibleChars >= m_totalChars) {
            visibleChars = m_totalChars;
            m_animating = false;
        }
        
        // 转换为字节索引
        displayByteLength = GetByteIndexForCharCount(text, visibleChars);
    }
    
    if (displayByteLength == 0) {
        canvas->restore();
        return;
    }
    
    // 获取字体度量
    SkFontMetrics metrics;
    font.getMetrics(&metrics);
    
    // 计算 X 坐标（水平对齐）
    float textX;
    if (horizontalAlign == Alignment::Left) {
        textX = X;
    } else {
        // 居中和右对齐需要测量宽度
        SkRect textBounds;
        font.measureText(text.c_str(), displayByteLength, 
                SkTextEncoding::kUTF8, &textBounds);
        if (horizontalAlign == Alignment::Center) {
            textX = X + (Width - textBounds.width()) / 2;
        } else {
            textX = X + Width - textBounds.width();
        }
    }
    
    // 计算 Y 坐标（垂直对齐）
    float textY;
    float textHeight = metrics.fDescent - metrics.fAscent;
    switch (verticalAlign) {
        case VerticalAlignment::Top:
            textY = Y - metrics.fAscent;
            break;
        case VerticalAlignment::Center:
            textY = Y + (Height - textHeight) / 2 - metrics.fAscent;
            break;
        case VerticalAlignment::Bottom:
            textY = Y + Height - metrics.fDescent;
            break;
    }
    
    // 直接绘制文本（drawSimpleText对动态长度文本更高效）
    canvas->drawSimpleText(text.c_str(), displayByteLength, SkTextEncoding::kUTF8,
                          textX, textY, font, paint);
    
    canvas->restore();
}

void TextBlock::startAnimation(bool loop) {
    m_loopAnimation = loop;
    
    // 计算UTF-8字符总数（不是字节数）
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

void TextBlock::stopAnimation() {
    m_animating = false;
}

void TextBlock::showFullText() {
    m_animating = false;
}

void TextBlock::clearText() {
    m_animating = false;
}
