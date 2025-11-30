#include "ui/TitleTransition.h"

void TitleTransition::Draw(IRenderer &r) {
    if (!IsVisible())
        return;

    r.drawRect(0.0f, 0.0f, (float)r.width(), (float)r.height(),
               Color{0, 0, 0, 255}, true);

    r.drawText(text, r.width() * 0.5f, r.height() * 0.5f, 68.0f,
               Color{255, 255, 255, 255}, TextHAlign::Center,
               TextVAlign::Middle);
}
