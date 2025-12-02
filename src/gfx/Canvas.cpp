#include "gfx/Canvas.h"

TextHAlign Canvas::toIRH(AlignH h) {
    switch (h) {
    case AlignH::Left:
        return TextHAlign::Left;
    case AlignH::Center:
        return TextHAlign::Center;
    case AlignH::Right:
        return TextHAlign::Right;
    }
    return TextHAlign::Left;
}

TextVAlign Canvas::toIRV(AlignV v) {
    switch (v) {
    case AlignV::Top:
        return TextVAlign::Top;
    case AlignV::Middle:
        return TextVAlign::Middle;
    case AlignV::Bottom:
        return TextVAlign::Bottom;
    case AlignV::Baseline:
        return TextVAlign::Baseline;
    }
    return TextVAlign::Baseline;
}

void Canvas::image(const ImageHandle &img, const gfx::Rect &dst, float alpha) {
    if (!img)
        return;
    gfx::Rect src = gfx::Rect::XYWH(0, 0, (float)img.width, (float)img.height);
    r.drawImageRect(img, src, dst, alpha);
}

void Canvas::text(const std::string &utf8, gfx::Vec2 pos,
                  const TextStyle &style, AlignH h, AlignV v) {
    if (style.stroke) {
        r.drawTextStroke(utf8, pos, style.sizePx, style.color,
                         style.strokeColor, style.strokeWidth, toIRH(h),
                         toIRV(v));
    } else {
        r.drawText(utf8, pos, style.sizePx, style.color, toIRH(h), toIRV(v));
    }
}

void Canvas::panel(const gfx::Rect &rc, float radius, Color bg, float borderW,
                   Color border) {
    if (borderW > 0.0f) {
        if (radius > 0.0f)
            r.drawRoundRect(rc, radius, border, true);
        else
            r.drawRect(rc, border, true);

        auto inner = rc.inset(borderW, borderW, borderW, borderW);
        float innerRadius = std::max(0.0f, radius - borderW);
        if (innerRadius > 0.0f)
            r.drawRoundRect(inner, innerRadius, bg, true);
        else
            r.drawRect(inner, bg, true);
    } else {
        if (radius > 0.0f)
            r.drawRoundRect(rc, radius, bg, true);
        else
            r.drawRect(rc, bg, true);
    }
}

void Canvas::textInRect(const std::string &utf8, const gfx::Rect &rc,
                        const TextStyle &style, AlignH h, AlignV v) {
    gfx::Vec2 anchor;
    anchor.x = (h == AlignH::Left)     ? rc.left()
               : (h == AlignH::Center) ? rc.center().x
                                       : rc.right();

    anchor.y = (v == AlignV::Top)      ? rc.top()
               : (v == AlignV::Middle) ? rc.center().y
               : (v == AlignV::Bottom) ? rc.bottom()
                                       : rc.center().y;

    text(utf8, anchor, style, h, v);
}

void Canvas::ninePatch(const ImageHandle &img, const gfx::Rect &dst,
                       const gfx::Insets &b, float alpha) {
    if (!img)
        return;

    float iw = (float)img.width;
    float ih = (float)img.height;

    gfx::Rect sTL = gfx::Rect::XYWH(0, 0, b.l, b.t);
    gfx::Rect sT = gfx::Rect::XYWH(b.l, 0, iw - b.l - b.r, b.t);
    gfx::Rect sTR = gfx::Rect::XYWH(iw - b.r, 0, b.r, b.t);

    gfx::Rect sL = gfx::Rect::XYWH(0, b.t, b.l, ih - b.t - b.b);
    gfx::Rect sC = gfx::Rect::XYWH(b.l, b.t, iw - b.l - b.r, ih - b.t - b.b);
    gfx::Rect sR = gfx::Rect::XYWH(iw - b.r, b.t, b.r, ih - b.t - b.b);

    gfx::Rect sBL = gfx::Rect::XYWH(0, ih - b.b, b.l, b.b);
    gfx::Rect sB = gfx::Rect::XYWH(b.l, ih - b.b, iw - b.l - b.r, b.b);
    gfx::Rect sBR = gfx::Rect::XYWH(iw - b.r, ih - b.b, b.r, b.b);

    float x = dst.x, y = dst.y, w = dst.w, h = dst.h;
    gfx::Rect dTL = gfx::Rect::XYWH(x, y, b.l, b.t);
    gfx::Rect dT = gfx::Rect::XYWH(x + b.l, y, w - b.l - b.r, b.t);
    gfx::Rect dTR = gfx::Rect::XYWH(x + w - b.r, y, b.r, b.t);

    gfx::Rect dL = gfx::Rect::XYWH(x, y + b.t, b.l, h - b.t - b.b);
    gfx::Rect dC =
        gfx::Rect::XYWH(x + b.l, y + b.t, w - b.l - b.r, h - b.t - b.b);
    gfx::Rect dR = gfx::Rect::XYWH(x + w - b.r, y + b.t, b.r, h - b.t - b.b);

    gfx::Rect dBL = gfx::Rect::XYWH(x, y + h - b.b, b.l, b.b);
    gfx::Rect dB = gfx::Rect::XYWH(x + b.l, y + h - b.b, w - b.l - b.r, b.b);
    gfx::Rect dBR = gfx::Rect::XYWH(x + w - b.r, y + h - b.b, b.r, b.b);

    image(img, sTL, dTL, alpha);
    image(img, sT, dT, alpha);
    image(img, sTR, dTR, alpha);
    image(img, sL, dL, alpha);
    image(img, sC, dC, alpha);
    image(img, sR, dR, alpha);
    image(img, sBL, dBL, alpha);
    image(img, sB, dB, alpha);
    image(img, sBR, dBR, alpha);
}
