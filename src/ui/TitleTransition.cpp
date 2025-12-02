#include "ui/TitleTransition.h"
#include "gfx/Canvas.h"

void TitleTransition::Draw(IRenderer &r) {
    if (!IsVisible())
        return;

    Canvas c(r);

    gfx::Rect full = gfx::Rect::XYWH(0, 0, (float)r.width(), (float)r.height());

    c.rect(full, Color{0, 0, 0, 255}, true);

    TextStyle ts;
    ts.sizePx = 68.0f;
    ts.color = Color{255, 255, 255, 255};

    c.textInRect(text, full, ts, AlignH::Center, AlignV::Middle);
}
