#include "windows/SettingWindow.h"
#include <GLFW/glfw3.h>

SettingWindow::SettingWindow(std::function<void()> bh, const std::string &text)
    : backHook(std::move(bh)) {
    tbk1 = std::make_shared<TextBlock>(text, 30, Color{0, 191, 255, 255});

    tbk1->SetFontStyle(1);
    tbk1->SetAlign(TextHAlign::Center, TextVAlign::Middle);
    tbk1->SetRect(Rect{0, 0, 1600, 900});

    btn1 = std::make_shared<PrimaryButton>("??", [this]() {
        if (backHook)
            backHook();
    });

    btn1->SetRect(Rect{100, 800, 120, 40});
    btn1->SetFontSize(16);

    AddMidgroundControl(tbk1);
    AddMidgroundControl(btn1);

    SetOnMouseRBUPHook([this]() {
        if (backHook)
            backHook();
    });

    SetOnKeyDownHook([this](int key, int) {
        if (key == GLFW_KEY_ESCAPE) {
            if (backHook)
                backHook();
        }
    });
}
