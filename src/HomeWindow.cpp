#include "HomeWindow.h"
#include "LoadingWindow.h"
#include "SettingWindow.h"
#include "IGlobalEvent.h"
#include <iostream>

HomeWindow::HomeWindow() {
    // 开始游戏按钮
    auto button2 = std::make_shared<PrimaryButton>("开始游戏", [this]() {
        std::cout << "开始游戏！" << std::endl;
        if (m_globalEvent) {
            // 创建LoadingWindow并设置过渡动画
            auto lw = std::make_shared<LoadingWindow>(
                std::vector<std::string>{"assets/image/misc/过渡-1.png"},
                700,  // 淡入时间
                700   // 淡出时间
            );
            
            // 设置动画完成后的回调：切换到游戏窗口
            lw->SetFinalHook([this]() {
                if (m_globalEvent) {
                    m_globalEvent->RequestWindowSwitch(2);
                }
            });
            
            // 先切换到LoadingWindow
            m_globalEvent->RequestWindowSwitch(lw);
            
            // 启动动画
            lw->StartAnimation();
        }
    });
    button2->SetPosition(60, 460);
    button2->SetSize(150, 30);
    button2->SetFontSize(18);
    button2->SetFontStyle(1);

    // 继续游戏按钮
    auto button3 = std::make_shared<PrimaryButton>("继续游戏", []() {
        std::cout << "继续！" << std::endl;
    });
    button3->SetPosition(60, 500);
    button3->SetSize(150, 30);
    button3->SetFontSize(18);

    // 设定按钮
    auto button4 = std::make_shared<PrimaryButton>("设定", [this]() {
        std::cout << "设定！" << std::endl;
        if (!m_globalEvent) {
            std::cerr << "错误：m_globalEvent 为空！" << std::endl;
            return;
        }
        
        // 使用局部变量保存 m_globalEvent 指针
        IGlobalEvent* ge = m_globalEvent;
        
        auto sw = std::make_shared<SettingWindow>(
            [ge]() { 
                std::cout << "返回回调被调用..." << std::endl;
                if (ge) {
                    std::cout << "切换到窗口 0" << std::endl;
                    ge->RequestWindowSwitch(0); 
                } else {
                    std::cerr << "错误：ge 指针为空！" << std::endl;
                }
            },
            "(ᵜʬᗜ) 很遗憾，我没写这个\n这游戏要什么设置"
        );
        std::cout << "准备切换到 SettingWindow..." << std::endl;
        m_globalEvent->RequestWindowSwitch(sw);
    });
    button4->SetPosition(60, 540);
    button4->SetSize(150, 30);
    button4->SetFontSize(18);

    // 关于按钮
    auto button5 = std::make_shared<PrimaryButton>("关于", []() {
        std::cout << "关于！" << std::endl;
    });
    button5->SetPosition(60, 580);
    button5->SetSize(150, 30);
    button5->SetFontSize(18);

    // Logo 图片
    auto ic = std::make_shared<ImageContainer>();
    ic->SetImageFromFile("assets/image/misc/DaysOfJxufe.png");
    ic->SetRectF(SkRect::MakeXYWH(880, 440, 300, 150));
    ic->SetMode(3);

    AddControl(button2);
    AddControl(button3);
    AddControl(button4);
    AddControl(button5);
    AddControl(ic);

    // 设置背景
    SetBackgroundImage("assets/image/background/教学楼-外-2.jpg");
}
