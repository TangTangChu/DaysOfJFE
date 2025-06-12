#pragma once
#include "stdafx.h"
#include "WindowPanel.h"
#include "Button.h"
#include "TextBlock.h"
#include <memory>
#include <functional>
class LoadSceneWindow :public WindowPanel {
private:
	std::shared_ptr<PrimaryButton> btn_1;
	std::shared_ptr<PrimaryButton> btn_2;
	std::shared_ptr<PrimaryButton> btn_3;
	std::shared_ptr<PrimaryButton> btn_4;
	std::shared_ptr<PrimaryButton> btn_5;
	std::shared_ptr<PrimaryButton> btn_bks;
	std::shared_ptr<TextBlock> tbk1;
	std::function<void()> backHook;
public:
	LoadSceneWindow(std::function<void()> b1, std::function<void()> b2, std::function<void()> b3,
		std::function<void()> b4, std::function<void()> b5,
		std::function<void()> bh);
};