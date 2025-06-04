#pragma once
#include "WindowPanel.h"
#include <functional>
#include <memory>
#include "ImageContainer.h"
#include <vector>

class LoadingWindow :public WindowPanel {
private:
	std::function<void()> FinalHook;
	std::shared_ptr<ImageContainer> bgimage;
	int fadeInTime, fadeOutTime;
	std::unique_ptr<std::vector<std::unique_ptr<Gdiplus::Image>>> imgList;
public:
	LoadingWindow(std::unique_ptr<std::vector<std::unique_ptr<Gdiplus::Image>>> imglist = nullptr, int fadeIn = 700, int fadeOut = 700);
	void SetFinalHook(std::function<void()> fh);
	void StartAnimation();
};