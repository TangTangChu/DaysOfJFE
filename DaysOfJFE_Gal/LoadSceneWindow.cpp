#include "stdafx.h"
#include "LoadSceneWindow.h"

LoadSceneWindow::LoadSceneWindow(std::function<void()> b1, std::function<void()> b2, std::function<void()> b3,
	std::function<void()> b4, std::function<void()> b5,
	std::function<void()> bh):backHook(bh) {
	btn_1 = std::make_shared<PrimaryButton>(L"场景1", b1);
	btn_2 = std::make_shared<PrimaryButton>(L"场景2", b2);
	btn_3 = std::make_shared<PrimaryButton>(L"场景3", b3);
	btn_4 = std::make_shared<PrimaryButton>(L"场景4", b4);
	btn_5 = std::make_shared<PrimaryButton>(L"场景5", b5);

	btn_1->SetPosition(40, 600);
	btn_1->SetSize(120, 40);
	btn_1->SetFontSize(24);

	btn_2->SetPosition(180, 600);
	btn_2->SetSize(120, 40);
	btn_2->SetFontSize(24);

	btn_3->SetPosition(320, 600);
	btn_3->SetSize(120, 40);
	btn_3->SetFontSize(24);

	btn_4->SetPosition(460, 600);
	btn_4->SetSize(120, 40);
	btn_4->SetFontSize(24);

	btn_5->SetPosition(600, 600);
	btn_5->SetSize(120, 40);
	btn_5->SetFontSize(24);

	auto btn_bk = std::make_shared<PrimaryButton>(L"返回", [this]() {if (backHook) backHook(); });
	btn_bk->SetPosition(40, 800);
	btn_bk->SetSize(200, 50);
	btn_bk->SetFontSize(25);
	tbk1 = std::make_shared<TextBlock>(L"我一共写了 5 个场景，你可以用下面的按钮调整至对应的场景", 30, Color(255, 0, 191, 255));
	auto customFormat = std::make_unique<StringFormat>();
	customFormat->SetAlignment(StringAlignmentCenter);
	customFormat->SetLineAlignment(StringAlignmentCenter);
	tbk1->SetStringFormat(std::move(customFormat));
	tbk1->SetRect(Rect{ 0,0,1600,900 });
	AddControl(tbk1);
	AddControl(btn_1);
	AddControl(btn_2);
	AddControl(btn_3);
	AddControl(btn_4);
	AddControl(btn_5);
	AddControl(btn_bk);
	
	OnMouseRBUPHook = [this]() {if (backHook) backHook(); };
	OnKeyDownHook = [this](ExMessage ex) {
		if (ex.vkcode == VK_ESCAPE) {
			if (backHook) backHook();
		}
		};
}

