#include "ui/Button.h"

void PrimaryButton::Draw(IRenderer &r) {
    if (!IsVisible())
        return;

    const float x = (float)GetX();
    const float y = (float)GetY();
    const float w = (float)GetWidth();
    const float h = (float)GetHeight();

    r.drawRRect(x, y, w, h, radius, finaBrush, true);

    r.drawText(text, x + w * 0.5f, y + h * 0.5f, (float)fontSize, textColor,
               TextHAlign::Center, TextVAlign::Middle);
}

void TextButton::Draw(IRenderer &r) {
    if (!IsVisible())
        return;

    const float x = (float)GetX();
    const float y = (float)GetY();
    const float w = (float)GetWidth();
    const float h = (float)GetHeight();

    r.drawText(text, x + w * 0.5f, y + h * 0.5f, (float)fontSize, textColor,
               TextHAlign::Center, TextVAlign::Middle);
}
