#include "windows/HomeWindow.h"
#include "audio/MusicPlayer.h"
#include "gfx/Assets.h"
#include "windows/LoadingWindow.h"
#include "windows/SettingWindow.h"

HomeWindow::HomeWindow() {
    auto startBtn = std::make_shared<PrimaryButton>("开始游戏", [this]() {
        if (!m_globalEvent)
            return;

        auto lw = std::make_shared<LoadingWindow>();
        lw->SetFinalHook([this]() { m_globalEvent->RequestWindowSwitch(1); });

        m_globalEvent->RequestWindowSwitch(lw);
        lw->StartAnimation();
    });

    startBtn->SetPosition(60, 460);
    startBtn->SetSize(150, 30);
    startBtn->SetFontSize(18);
    startBtn->SetFontStyle(1);

    auto contBtn = std::make_shared<PrimaryButton>("继续游戏", []() {});
    contBtn->SetPosition(60, 500);
    contBtn->SetSize(150, 30);
    contBtn->SetFontSize(18);

    auto settingBtn = std::make_shared<PrimaryButton>("设定", [this]() {
        if (!m_globalEvent)
            return;

        auto backToHome = [this]() { m_globalEvent->RequestWindowSwitch(0); };
        auto sw = std::make_shared<SettingWindow>(
            backToHome, "(ᗜˬᗜ) 很遗憾，我没写这个\n这游戏要什么设置");
        m_globalEvent->RequestWindowSwitch(sw);
    });

    settingBtn->SetPosition(60, 540);
    settingBtn->SetSize(150, 30);
    settingBtn->SetFontSize(18);

    auto aboutBtn = std::make_shared<PrimaryButton>("关于", []() {});
    aboutBtn->SetPosition(60, 580);
    aboutBtn->SetSize(150, 30);
    aboutBtn->SetFontSize(18);

    auto ic = std::make_shared<ImageContainer>();
    ImageHandle logo = Assets::LoadImage("assets/image/misc/DaysOfJxufe.png");
    ic->SetImage(logo);
    ic->SetRectF({880, 440, 300, 150});
    ic->SetMode(3);

    AddMidgroundControl(startBtn);
    AddMidgroundControl(contBtn);
    AddMidgroundControl(settingBtn);
    AddMidgroundControl(aboutBtn);
    AddMidgroundControl(ic);

    ImageHandle bg =
        Assets::LoadImage("assets/image/background/教学楼-外-2.jpg");
    SetBackground(bg);
}
