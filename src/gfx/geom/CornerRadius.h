#pragma once
namespace gfx {

struct CornerRadius {
    float tl = 0, tr = 0, br = 0, bl = 0;
    static CornerRadius All(float r) { return {r, r, r, r}; }
};

} // namespace gfx
