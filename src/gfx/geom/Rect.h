#pragma once
#include "gfx/geom/Vec2.h"
struct Rect {
    float x, y, w, h;
    static Rect XYWH(float x, float y, float w, float h);
    float left() const;
    float right() const;
    float top() const;
    float bottom() const;
    Vec2 center() const;
    bool contains(Vec2 p) const;
    Rect inset(float l, float t, float r, float b) const;
    Rect offset(Vec2 d) const;
};