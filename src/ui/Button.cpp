#include "ui/Button.h"
#include "gfx/Canvas.h"

void PrimaryButton::Draw(IRenderer &r) {
    if (!IsVisible())
        return;

    Canvas c(r);

    c.rrect(bounds, radius, finaBrush, true);

    TextStyle ts;
    ts.sizePx = (float)fontSize;
    ts.color = textColor;

    c.textInRect(text, bounds, ts, AlignH::Center, AlignV::Middle);
}

void TextButton::Draw(IRenderer &r) {
    if (!IsVisible())
        return;

    Canvas c(r);

    TextStyle ts;
    ts.sizePx = (float)fontSize;
    ts.color = textColor;

    c.textInRect(text, bounds, ts, AlignH::Center, AlignV::Middle);
}
