#pragma once
#include "WindowPanel.h"
#include "Button.h"
#include "TextBlock.h"
using namespace std;
class SettingWindow :public WindowPanel {
private:
	shared_ptr<TextBlock> tbk1;
	shared_ptr<PrimaryButton> btn1;
	function<void()> backHook;
public:
	SettingWindow(function<void()> bh, std::wstring text);
};