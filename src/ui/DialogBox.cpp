#include "DialogBox.h"
#include "gfx/Canvas.h"

DialogBox::DialogBox() {
    nameBlock = std::make_shared<TextBlock>("", 24, Color{255, 255, 100, 255});
    nameBlock->SetFontStyle(1);

    msgBlock = std::make_shared<TextBlock>("", 28, Color{255, 255, 255, 255});
    msgBlock->SetFontStyle(1);

    AddChild(nameBlock);
    AddChild(msgBlock);
}

void DialogBox::SetName(const std::string &name) { nameBlock->SetText(name); }

void DialogBox::SetText(const std::string &text) {
    msgBlock->SetText(text);
    msgBlock->startAnimation(false);
}

void DialogBox::SetTextImmediate(const std::string &text) {
    msgBlock->SetText(text);
    msgBlock->showFullText();
}

void DialogBox::ClearText() { msgBlock->clearText(); }

bool DialogBox::IsAnimating() const { return msgBlock->IsAnimating(); }

void DialogBox::SkipAnimation() { msgBlock->showFullText(); }

void DialogBox::UpdateLayout() {
    if (!nameBlock || !msgBlock)
        return;

    nameBlock->SetPosition(bounds.x + padding, bounds.y + padding / 2);
    nameBlock->SetSize(bounds.w - 2 * padding, 40);

    msgBlock->SetPosition(bounds.x + padding, bounds.y + padding + 40);
    msgBlock->SetSize(bounds.w - 2 * padding,
                      bounds.h - (padding + 40 + padding));
}

void DialogBox::Draw(IRenderer &r) {
    Canvas c(r);
    c.panel(bounds, 15.0f, bgColor, 2.0f, borderColor);

    LayoutControl::Draw(r);
}
