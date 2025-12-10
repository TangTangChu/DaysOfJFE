#pragma once
#include "WindowPanel.h"
#include "Button.h"
#include "TextBlock.h"
#include <functional>
#include <memory>
#include <string>

class SettingWindow : public WindowPanel {
private:
    std::shared_ptr<TextBlock> tbk1;
    std::shared_ptr<PrimaryButton> btn1;
    std::function<void()> backHook;

public:
    SettingWindow(std::function<void()> bh, const std::string& text);
};
