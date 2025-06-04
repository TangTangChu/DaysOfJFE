#include "stdafx.h"
#include "HomeWindow.h"
#include "LoadingWindow.h"
#include "SettingWindow.h"
#include <chrono>
#include <thread>
#include "MusicPlayer.h"

HomeWindow::HomeWindow()
{
	auto button2 = std::make_shared<PrimaryButton>(L"开始游戏", [this]()
		{
			std::cout << "开始游戏！" << std::endl;
			if (m_globalEvent)
			{
				//m_globalEvent->RequestWindowSwitch(1);
				auto lw = std::make_shared<LoadingWindow>();
				lw->SetFinalHook([this]() {
					m_globalEvent->RequestWindowSwitch(1);
					});
				m_globalEvent->RequestWindowSwitch(lw);
				lw->StartAnimation();
			}
			// 调用切换函数
		});
	button2->SetPosition(60, 460);
	button2->SetSize(150, 30);
	button2->SetFontSize(18);
	button2->SetFontStyle(1);

	auto button3 = std::make_shared<PrimaryButton>(L"继续游戏", []()
		{ std::cout << "继续！" << std::endl; });
	button3->SetPosition(60, 500);
	button3->SetSize(150, 30);
	button3->SetFontSize(18);

	auto button4 = std::make_shared<PrimaryButton>(L"设定", [this]()
		{
			std::cout << "设定！" << std::endl;
			auto sw = std::make_shared<SettingWindow>([this]() {m_globalEvent->RequestWindowSwitch(0); }, L"(ᗜˬᗜ) 很遗憾，我没写这个\n这游戏要什么设置");
			m_globalEvent->RequestWindowSwitch(sw);
		});
	button4->SetPosition(60, 540);
	button4->SetSize(150, 30);
	button4->SetFontSize(18);

	auto button5 = std::make_shared<PrimaryButton>(L"关于", []()
		{ std::cout << "关于！" << std::endl; });
	button5->SetPosition(60, 580);
	button5->SetSize(150, 30);
	button5->SetFontSize(18);
	auto ic = std::make_shared<ImageContainer>();
	auto logo = std::make_unique<Image>(L"src/image/misc/DaysOfJxufe.png");
	ic->SetImage(std::move(logo));
	RectF rectf(880, 440, 300, 150);
	ic->SetRectF(rectf);
	ic->SetMode(3);

	AddControl(button2);
	AddControl(button3);
	AddControl(button4);
	AddControl(button5);
	AddControl(ic);
	Image img3 = Image(L"src/image/background/教学楼-外-2.jpg");
	SetBackground(img3);
}