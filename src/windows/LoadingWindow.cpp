#include "windows/LoadingWindow.h"
#include <chrono>
#include <thread>

LoadingWindow::LoadingWindow(std::vector<ImageHandle> images, int fadeIn,
                             int fadeOut)
    : fadeInTime(fadeIn), fadeOutTime(fadeOut), imgList(std::move(images)) {}

void LoadingWindow::SetFinalHook(std::function<void()> fh) {
    FinalHook = std::move(fh);
}

void LoadingWindow::StartAnimation() {
    std::thread([this]() {
        constexpr int frameInterval = 16;

        for (auto &img : imgList) {
            if (!img)
                continue;

            SetBackground(img);

            auto start = std::chrono::steady_clock::now();
            while (true) {
                auto elapsed =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start)
                        .count();
                if (elapsed >= fadeInTime)
                    break;

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(frameInterval));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            start = std::chrono::steady_clock::now();
            while (true) {
                auto elapsed =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start)
                        .count();
                if (elapsed >= fadeOutTime)
                    break;

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(frameInterval));
            }
        }

        if (FinalHook)
            FinalHook();
    }).detach();
}
