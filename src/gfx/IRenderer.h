#pragma once
#include "include/core/SkImage.h"
#include <cstdint>
#include <memory>
#include <string>

struct Color {
    uint8_t r, g, b, a;
};

struct ImageHandle {
    sk_sp<SkImage> image;
    int width = 0;
    int height = 0;
    explicit operator bool() const { return (bool)image; }
};

struct CornerRadius {
    float A = 0, B = 0, C = 0, D = 0; // 左上, 右上, 右下, 左下
};

enum class TextHAlign { Left, Center, Right };
enum class TextVAlign { Top, Middle, Bottom, Baseline };

class IRenderer {
  public:
    virtual ~IRenderer() = default;

    virtual int width() const = 0;
    virtual int height() const = 0;

    virtual void clear(Color c) = 0;

    virtual void drawImage(const ImageHandle &img, float sx, float sy, float sw,
                           float sh, float dx, float dy, float dw, float dh,
                           float alpha = 1.0f) = 0;

    virtual void drawRect(float x, float y, float w, float h, Color c,
                          bool filled = true, float strokeWidth = 1.0f) = 0;

    virtual void drawRoundRect(float x, float y, float w, float h, float radius,
                               Color c, bool filled = true,
                               float strokeWidth = 1.0f) = 0;

    virtual void drawRRect(float x, float y, float w, float h, CornerRadius rad,
                           Color c, bool filled = true,
                           float strokeWidth = 1.0f) = 0;

    virtual void drawLine(float x1, float y1, float x2, float y2, Color c,
                          float strokeWidth = 1.0f) = 0;

    virtual void drawCircle(float cx, float cy, float r, Color c,
                            bool filled = true, float strokeWidth = 1.0f) = 0;

    virtual void drawText(const std::string &utf8, float x, float y,
                          float sizePx, Color c,
                          TextHAlign hAlign = TextHAlign::Left,
                          TextVAlign vAlign = TextVAlign::Baseline) = 0;

    virtual void drawTextStroke(const std::string &utf8, float x, float y,
                                float sizePx, Color fill, Color stroke,
                                float strokeWidth,
                                TextHAlign hAlign = TextHAlign::Left,
                                TextVAlign vAlign = TextVAlign::Baseline) = 0;
};
