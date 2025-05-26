#pragma once
#include "Controls.h"
#include <string>
#include <functional>
class Dialog:public Controls {
private:
	std::wstring text;
	std::function<void()> yesHook;
	std::function<void()> noHook;
public:
	Dialog(std::wstring text, std::function<void()> yesHook, std::function<void()> noHook);
};