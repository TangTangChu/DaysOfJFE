#include "stdafx.h"
#include "SettingWindow.h"

SettingWindow::SettingWindow(function<void()> bh, std::wstring text) :backHook(bh) {
	tbk1 = make_shared<TextBlock>(text.c_str(), 30, Color(255, 0, 191, 255));
	tbk1->SetFontStyle(1);
	auto customFormat = std::make_unique<StringFormat>();
	customFormat->SetAlignment(StringAlignmentCenter);
	customFormat->SetLineAlignment(StringAlignmentCenter);
	tbk1->SetStringFormat(std::move(customFormat));
	tbk1->SetRect(Rect{ 0,0,1600,900 });
	OnMouseRBUPHook = [this]() {if (backHook) backHook(); };
	OnKeyDownHook = [this](ExMessage ex) {
		if (ex.vkcode == VK_ESCAPE) {
			if (backHook) backHook();
		}
		};
	btn1 = make_shared<PrimaryButton>(L"·µ»Ø", [this]() {if (backHook) backHook(); });
	btn1->SetRect(Rect{ 100,800,120,40 });
	btn1->SetFontSize(16);
	AddControl(tbk1);
	AddControl(btn1);
}