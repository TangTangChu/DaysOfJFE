#include "LoadingWindow.h"
#include <chrono>
#include <thread>

LoadingWindow::LoadingWindow(std::vector<std::string> imglist, int fadeIn, int fadeOut)
    : imgList(std::move(imglist)), bgimage(std::make_shared<ImageContainer>()), 
      fadeInTime(fadeIn), fadeOutTime(fadeOut) {
    BgAlpha = 0;
}

void LoadingWindow::SetFinalHook(std::function<void()> fh) {
    this->FinalHook = fh;
}

void LoadingWindow::StartAnimation() {
    std::thread([this]() {
        constexpr int frameInterval = 16;
        
        if (!imgList.empty()) {
            for (const auto& imgPath : imgList) {
                BgAlpha = 0;
                SetBackgroundImage(imgPath);
                
                // 淡入
                auto start = std::chrono::steady_clock::now();
                while (true) {
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start).count();

                    if (elapsed >= fadeInTime) break;

                    BgAlpha = static_cast<uint8_t>(255 * elapsed / fadeInTime);
                    std::this_thread::sleep_for(std::chrono::milliseconds(frameInterval));
                }
                BgAlpha = 255;

                // 显示
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                // 淡出
                start = std::chrono::steady_clock::now();
                while (true) {
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start).count();

                    if (elapsed >= fadeOutTime) break;

                    BgAlpha = static_cast<uint8_t>(255 - (255 * elapsed / fadeOutTime));
                    std::this_thread::sleep_for(std::chrono::milliseconds(frameInterval));
                }
                BgAlpha = 0;
            }
        }

        if (FinalHook) {
            FinalHook();
        }
    }).detach();
}
