#include "stdafx.h"
#include "LoadingWindow.h"
#include <chrono>
#include <thread>

LoadingWindow::LoadingWindow(std::unique_ptr<std::vector<std::unique_ptr<Gdiplus::Image>>> imglist, int fadeIn, int fadeOut) :
	imgList(std::move(imglist)), bgimage(new ImageContainer()), fadeInTime(fadeIn), fadeOutTime(fadeOut)
{
	BgAlpha = 0;
	if (!imgList) {
		imgList = std::make_unique<std::vector<std::unique_ptr<Gdiplus::Image>>>();
	}
}
void LoadingWindow::SetFinalHook(std::function<void()> fh) {
	this->FinalHook = fh;
}

void LoadingWindow::StartAnimation() {
	std::thread([this]() {
		constexpr int frameInterval = 16;
		if (imgList) {
			for (auto& img : *imgList) {
				if (img) {
					BgAlpha = 0;
					SetBackground(*img);
					auto start = std::chrono::steady_clock::now();
					while (true) {
						auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
							std::chrono::steady_clock::now() - start).count();

						if (elapsed >= fadeInTime) break;

						BgAlpha = static_cast<BYTE>(255 * elapsed / fadeInTime);
						std::this_thread::sleep_for(std::chrono::milliseconds(frameInterval));
					}
					BgAlpha = 255;

					std::this_thread::sleep_for(std::chrono::milliseconds(1000));

					// µ­³ö
					start = std::chrono::steady_clock::now();
					while (true) {
						auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
							std::chrono::steady_clock::now() - start).count();

						if (elapsed >= fadeOutTime) break;

						BgAlpha = static_cast<BYTE>(255 - (255 * elapsed / fadeOutTime));
						std::this_thread::sleep_for(std::chrono::milliseconds(frameInterval));
					}
					BgAlpha = 0;
				}
			}
		}

		if (FinalHook) { FinalHook(); }
		}).detach();
}