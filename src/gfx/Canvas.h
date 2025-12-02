#pragma once
#include "gfx/IRenderer.h"
#include "gfx/geom/Geom.h"
#include <algorithm>
#include <string>

enum class AlignH { Left, Center, Right };
enum class AlignV { Top, Middle, Bottom, Baseline };

struct TextStyle {
    float sizePx = 18.0f;
    Color color{255, 255, 255, 255};

    bool stroke = false;
    Color strokeColor{0, 0, 0, 255};
    float strokeWidth = 1.0f;
};

class Canvas {
  public:
    explicit Canvas(IRenderer &r) : r(r) {}
    void clear(Color c) { r.clear(c); }

    void rect(const gfx::Rect &rc, Color c, bool filled = true,
              float sw = 1.0f) {
        r.drawRect(rc, c, filled, sw);
    }

    void roundRect(const gfx::Rect &rc, float radius, Color c,
                   bool filled = true, float sw = 1.0f) {
        r.drawRoundRect(rc, radius, c, filled, sw);
    }

    void rrect(const gfx::Rect &rc, const gfx::CornerRadius &rad, Color c,
               bool filled = true, float sw = 1.0f) {
        r.drawRRect(rc, rad, c, filled, sw);
    }

    void line(gfx::Vec2 a, gfx::Vec2 b, Color c, float sw = 1.0f) {
        r.drawLine(a, b, c, sw);
    }

    void circle(gfx::Vec2 center, float radius, Color c, bool filled = true,
                float sw = 1.0f) {
        r.drawCircle(center, radius, c, filled, sw);
    }

    void image(const ImageHandle &img, const gfx::Rect &dst,
               float alpha = 1.0f);
    void image(const ImageHandle &img, const gfx::Rect &src,
               const gfx::Rect &dst, float alpha = 1.0f) {
        r.drawImageRect(img, src, dst, alpha);
    }

    void text(const std::string &utf8, gfx::Vec2 pos, const TextStyle &style,
              AlignH h = AlignH::Left, AlignV v = AlignV::Baseline);

    void panel(const gfx::Rect &rc, float radius, Color bg,
               float borderW = 0.0f, Color border = {0, 0, 0, 0});

    void textInRect(const std::string &utf8, const gfx::Rect &rc,
                    const TextStyle &style, AlignH h = AlignH::Center,
                    AlignV v = AlignV::Middle);

    void ninePatch(const ImageHandle &img, const gfx::Rect &dst,
                   const gfx::Insets &border, float alpha = 1.0f);

  private:
    IRenderer &r;

    static TextHAlign toIRH(AlignH h);
    static TextVAlign toIRV(AlignV v);
};
