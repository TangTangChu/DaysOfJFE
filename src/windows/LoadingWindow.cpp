#include "windows/LoadingWindow.h"
#include <chrono>
#include <thread>

LoadingWindow::LoadingWindow(std::vector<ImageHandle> images, int fadeIn,
                             int fadeOut)
    : fadeInTime(fadeIn), fadeOutTime(fadeOut), imgList(std::move(images)) {}

void LoadingWindow::SetFinalHook(std::function<void()> fh) {
    FinalHook = std::move(fh);
}
void LoadingWindow::DrawCustomBackground(Canvas &c, IRenderer &r) {
    auto *bgp = GetBackground();
    if (!bgp)
        return;
    const auto &bg = *bgp;

    float scale = std::min((float)r.width() / (float)bg.width,
                           (float)r.height() / (float)bg.height);
    float w = bg.width * scale;
    float h = bg.height * scale;
    float x = (r.width() - w) * 0.5f;
    float y = (r.height() - h) * 0.5f;

    gfx::Rect src = gfx::Rect::XYWH(0, 0, (float)bg.width, (float)bg.height);
    gfx::Rect dst = gfx::Rect::XYWH(x, y, w, h);

    c.image(bg, src, dst, GetBackgroundAlpha());
}

void LoadingWindow::StartAnimation() {
    std::thread([this]() {
        constexpr int frameInterval = 16;

        for (auto &img : imgList) {
            if (!img)
                continue;

            SetBackground(img);

            // fade in
            bgAlpha.store(0.0f);
            auto start = std::chrono::steady_clock::now();
            while (true) {
                auto elapsed =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start)
                        .count();
                float t = std::min(1.0f, elapsed / (float)fadeInTime);
                bgAlpha.store(t);
                if (t >= 1.0f)
                    break;
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(frameInterval));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            // fade out
            start = std::chrono::steady_clock::now();
            while (true) {
                auto elapsed =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start)
                        .count();
                float t = std::min(1.0f, elapsed / (float)fadeOutTime);
                bgAlpha.store(1.0f - t);
                if (t >= 1.0f)
                    break;
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(frameInterval));
            }
        }

        if (FinalHook)
            FinalHook();
    }).detach();
}
