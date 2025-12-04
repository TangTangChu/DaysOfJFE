#include "windows/WindowManager.h"
#include "app/ApplicationContext.h"
#include "gfx/Canvas.h"
#include "ui/Controls.h"
#include "windows/WindowPanel.h"
#include <algorithm>


void WindowManager::SetGlobalEvent(ApplicationContext *g) {
    applicationContext = g;
    for (auto &window : windows) {
        if (window) {
            window->SetGlobalEvent(g);
        }
    }
    if (currentWindow) {
        currentWindow->SetGlobalEvent(g);
    }
}

void WindowManager::AddWindow(std::shared_ptr<WindowPanel> panel) {
    if (!panel)
        return;
    panel->SetGlobalEvent(applicationContext);
    windows.push_back(panel);

    if (!currentWindow) {
        currentWindow = panel;
    }
}

void WindowManager::SwitchWindow(int index) {
    if (index < 0 || index >= (int)windows.size())
        return;
    currentWindow = windows[(size_t)index];
    if (currentWindow) {
        currentWindow->SetGlobalEvent(applicationContext);
    }
}

std::shared_ptr<WindowPanel> WindowManager::GetCurrentWindow() const {
    return currentWindow;
}

bool WindowManager::AddControl(std::shared_ptr<Controls> ctrl, Layer layer) {
    if (!currentWindow || !ctrl)
        return false;
    switch (layer) {
    case Layer::BACKGROUND:
        currentWindow->AddBackgroundControl(std::move(ctrl));
        break;
    case Layer::MIDGROUND:
        currentWindow->AddMidgroundControl(std::move(ctrl));
        break;
    case Layer::FOREGROUND:
        currentWindow->AddForegroundControl(std::move(ctrl));
        break;
    }
    return true;
}

bool WindowManager::RemoveControl(const std::shared_ptr<Controls> &ctrl) {
    if (!currentWindow)
        return false;
    return currentWindow->RemoveControl(ctrl);
}

void WindowManager::ClearAllControls() {
    if (currentWindow)
        currentWindow->ClearAllControls();
}

void WindowManager::SetBackground(const ImageHandle &img) {
    if (currentWindow)
        currentWindow->SetBackground(img);
}

void WindowManager::ClearBackground() {
    if (currentWindow)
        currentWindow->ClearBackground();
}

void WindowManager::Redraw(IRenderer &r) {
    if (!currentWindow)
        return;

    r.clear(Color{255, 255, 255, 255});

    Canvas c(r);

    if (currentWindow->HasCustomBackground()) {
        currentWindow->DrawCustomBackground(c, r);
    } else if (auto *bgp = currentWindow->GetBackground()) {
        const auto &bg = *bgp;

        float scale = std::min((float)r.width() / (float)bg.width,
                               (float)r.height() / (float)bg.height);
        float w = bg.width * scale;
        float h = bg.height * scale;
        float x = (r.width() - w) * 0.5f;
        float y = (r.height() - h) * 0.5f;

        gfx::Rect src =
            gfx::Rect::XYWH(0, 0, (float)bg.width, (float)bg.height);
        gfx::Rect dst = gfx::Rect::XYWH(x, y, w, h);

        c.image(bg, src, dst, 1.0f);
    }

    auto controls = currentWindow->GetAllControls();
    for (auto &c : controls) {
        if (c && c->IsVisible()) {
            c->Draw(r);
        }
    }
}

void WindowManager::Dispatch(const PlatformEvent &e) {
    if (currentWindow) {
        currentWindow->HandleEvent(e);
    }
}

void WindowManager::ShowDialog(std::wstring, std::function<void()>,
                               std::function<void()>) {}

void WindowManager::CloseDialog() {}

void WindowManager::SwitchWindow(std::shared_ptr<WindowPanel> window) {
    if (!window)
        return;

    // 如果不在 windows 里
    auto it = std::find(windows.begin(), windows.end(), window);
    if (it == windows.end()) {
        window->SetGlobalEvent(applicationContext);
        windows.push_back(window);
        currentWindow = window;
        return;
    }

    currentWindow = *it;
    if (currentWindow)
        currentWindow->SetGlobalEvent(applicationContext);
}

std::shared_ptr<WindowPanel> WindowManager::GetWindow(int index) const {
    if (index < 0 || index >= (int)windows.size())
        return nullptr;
    return windows[(size_t)index];
}
