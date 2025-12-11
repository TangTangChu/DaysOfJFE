#include "windows/SettingWindow.h"
#include "ui/Button.h"
#include "ui/TextBlock.h"

SettingWindow::SettingWindow(std::function<void()> bh,
                             const std::string &text) {
    contentStack =
        std::make_shared<StackPanel>(StackPanel::Orientation::Vertical);
    contentStack->SetSpacing(20);
    contentStack->SetAlignment(StackPanel::Alignment::Center);

    auto msg = std::make_shared<TextBlock>(text, 24);
    msg->SetAlign(TextHAlign::Center, TextVAlign::Middle);
    msg->SetSize(800, 400);

    contentStack->AddChild(msg);

    auto backBtn = std::make_shared<PrimaryButton>("返回", bh);
    backBtn->SetSize(200, 50);
    backBtn->SetFontSize(24);
    contentStack->AddChild(backBtn);
    contentStack->SetBounds(gfx::Rect::XYWH(200, 100, 800, 500));

    AddForegroundControl(contentStack);
}

void SettingWindow::OnWindowResize(int width, int height) {
    if (contentStack) {
        int w = 800;
        int h = 500;
        contentStack->SetBounds(
            gfx::Rect::XYWH((width - w) / 2, (height - h) / 2, w, h));
        contentStack->UpdateLayout();
    }
    WindowPanel::OnWindowResize(width, height);
}

void SettingWindow::DrawCustomBackground(Canvas &c, IRenderer &r) {
    c.rect(gfx::Rect::XYWH(0, 0, (float)r.width(), (float)r.height()), bgColor);
}
