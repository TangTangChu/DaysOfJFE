#pragma once
#include "gfx/geom/Geom.h"
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

enum class TextHAlign { Left, Center, Right };
enum class TextVAlign { Top, Middle, Bottom, Baseline };
struct TextMetrics {
    float w = 0;
    float h = 0;
    float ascent = 0;
    float descent = 0;
};

class IRenderer {
  public:
    virtual ~IRenderer() = default;
    virtual int width() const = 0;
    virtual int height() const = 0;
    virtual void clear(Color c) = 0;

    virtual void drawRect(const gfx::Rect &rc, Color c, bool filled = true,
                          float strokeWidth = 1.0f) = 0;

    virtual void drawRoundRect(const gfx::Rect &rc, float radius, Color c,
                               bool filled = true,
                               float strokeWidth = 1.0f) = 0;

    virtual void drawRRect(const gfx::Rect &rc, const gfx::CornerRadius &rad,
                           Color c, bool filled = true,
                           float strokeWidth = 1.0f) = 0;

    virtual void drawLine(gfx::Vec2 a, gfx::Vec2 b, Color c,
                          float strokeWidth = 1.0f) = 0;

    virtual void drawCircle(gfx::Vec2 center, float r, Color c,
                            bool filled = true, float strokeWidth = 1.0f) = 0;

    virtual void drawImageRect(const ImageHandle &img, const gfx::Rect &src,
                               const gfx::Rect &dst, float alpha = 1.0f) = 0;

    virtual TextMetrics measureText(const std::string &utf8, float sizePx) = 0;

    virtual void drawText(const std::string &utf8, gfx::Vec2 pos, float sizePx,
                          Color c, TextHAlign hAlign = TextHAlign::Left,
                          TextVAlign vAlign = TextVAlign::Baseline) = 0;

    virtual void drawTextStroke(const std::string &utf8, gfx::Vec2 pos,
                                float sizePx, Color fill, Color stroke,
                                float strokeWidth,
                                TextHAlign hAlign = TextHAlign::Left,
                                TextVAlign vAlign = TextVAlign::Baseline) = 0;
};
