#include "gfx/geom/Rect.h"

namespace gfx {

Rect Rect::XYWH(float x, float y, float w, float h) { return {x, y, w, h}; }

float Rect::left() const { return x; }
float Rect::right() const { return x + w; }
float Rect::top() const { return y; }
float Rect::bottom() const { return y + h; }

Vec2 Rect::center() const { return {x + w * 0.5f, y + h * 0.5f}; }

bool Rect::contains(Vec2 p) const {
    return p.x >= left() && p.x <= right() && p.y >= top() && p.y <= bottom();
}

Rect Rect::inset(float l, float t, float r, float b) const {
    return {x + l, y + t, w - l - r, h - t - b};
}

Rect Rect::offset(Vec2 d) const { return {x + d.x, y + d.y, w, h}; }

} // namespace gfx
