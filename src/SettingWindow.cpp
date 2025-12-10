#include "SettingWindow.h"
#include <SDL2/SDL.h>
#include <iostream>

SettingWindow::SettingWindow(std::function<void()> bh, const std::string& text)
    : backHook(bh) {
    
    // 设置背景色以便看清文本
    BgAlpha = 200;
    
    tbk1 = std::make_shared<TextBlock>(text, 30, SK_ColorCYAN);
    tbk1->SetFontStyle(1);
    tbk1->SetHorizontalAlignment(TextBlock::Alignment::Center);
    tbk1->SetVerticalAlignment(TextBlock::VerticalAlignment::Center);
    tbk1->SetPosition(0, 0);
    tbk1->SetSize(1600, 900);
    
    std::cout << "SettingWindow 创建，文本长度: " << text.length() << std::endl;
    
    // 右键或 ESC 返回 - 使用 shared_ptr 保存 backHook 副本
    auto hookCopy = std::make_shared<std::function<void()>>(backHook);
    
    OnMouseRBUPHook = [hookCopy]() {
        std::cout << "右键点击，准备返回..." << std::endl;
        if (hookCopy && *hookCopy) {
            (*hookCopy)();
        }
    };
    
    OnKeyDownHook = [hookCopy](SDL_Keycode keycode) {
        if (keycode == SDLK_ESCAPE) {
            std::cout << "ESC 键按下，准备返回..." << std::endl;
            if (hookCopy && *hookCopy) {
                (*hookCopy)();
            }
        }
    };
    
    btn1 = std::make_shared<PrimaryButton>("返回", [hookCopy]() {
        std::cout << "返回按钮点击，准备返回..." << std::endl;
        if (hookCopy && *hookCopy) {
            (*hookCopy)();
        }
    });
    btn1->SetPosition(100, 800);
    btn1->SetSize(120, 40);
    btn1->SetFontSize(16);
    
    AddControl(tbk1);
    AddControl(btn1);
}
