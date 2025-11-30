#include "ui/WindowPanel.h"
#include <algorithm>

void WindowPanel::markDirty() const { controlsDirty = true; }

void WindowPanel::rebuildCache() const {
    allControlsCache.clear();
    allControlsCache.reserve(backgroundControls.size() +
                             midgroundControls.size() +
                             foregroundControls.size());

    allControlsCache.insert(allControlsCache.end(), backgroundControls.begin(),
                            backgroundControls.end());
    allControlsCache.insert(allControlsCache.end(), midgroundControls.begin(),
                            midgroundControls.end());
    allControlsCache.insert(allControlsCache.end(), foregroundControls.begin(),
                            foregroundControls.end());
}

const std::vector<std::shared_ptr<Controls>> &
WindowPanel::getAllControlsCached() const {
    if (controlsDirty) {
        rebuildCache();
        controlsDirty = false;
    }
    return allControlsCache;
}

void WindowPanel::SetGlobalEvent(IGlobalEvent *globalEvent) {
    m_globalEvent = globalEvent;
}

void WindowPanel::AddBackgroundControl(std::shared_ptr<Controls> ctrl) {
    if (!ctrl)
        return;
    backgroundControls.push_back(std::move(ctrl));
    markDirty();
}

void WindowPanel::AddMidgroundControl(std::shared_ptr<Controls> ctrl) {
    if (!ctrl)
        return;
    midgroundControls.push_back(std::move(ctrl));
    markDirty();
}

void WindowPanel::AddForegroundControl(std::shared_ptr<Controls> ctrl) {
    if (!ctrl)
        return;
    foregroundControls.push_back(std::move(ctrl));
    markDirty();
}

bool WindowPanel::RemoveControl(const std::shared_ptr<Controls> &ctrl) {
    if (!ctrl)
        return false;

    auto removeFrom = [&](std::vector<std::shared_ptr<Controls>> &vec) {
        auto it = std::remove(vec.begin(), vec.end(), ctrl);
        bool removed = it != vec.end();
        vec.erase(it, vec.end());
        return removed;
    };

    bool removed = removeFrom(backgroundControls) ||
                   removeFrom(midgroundControls) ||
                   removeFrom(foregroundControls);

    if (removed) {
        if (auto p = pressedControl.lock()) {
            if (p == ctrl)
                pressedControl.reset();
        }
        markDirty();
    }

    return removed;
}

bool WindowPanel::RemoveBackgroundControl(std::shared_ptr<Controls> ctrl) {
    if (!ctrl)
        return false;
    auto it =
        std::remove(backgroundControls.begin(), backgroundControls.end(), ctrl);
    bool removed = it != backgroundControls.end();
    backgroundControls.erase(it, backgroundControls.end());
    if (removed) {
        if (auto p = pressedControl.lock()) {
            if (p == ctrl)
                pressedControl.reset();
        }
        markDirty();
    }
    return removed;
}

bool WindowPanel::RemoveMidgroundControl(std::shared_ptr<Controls> ctrl) {
    if (!ctrl)
        return false;
    auto it =
        std::remove(midgroundControls.begin(), midgroundControls.end(), ctrl);
    bool removed = it != midgroundControls.end();
    midgroundControls.erase(it, midgroundControls.end());
    if (removed) {
        if (auto p = pressedControl.lock()) {
            if (p == ctrl)
                pressedControl.reset();
        }
        markDirty();
    }
    return removed;
}

bool WindowPanel::RemoveForegroundControl(std::shared_ptr<Controls> ctrl) {
    if (!ctrl)
        return false;
    auto it =
        std::remove(foregroundControls.begin(), foregroundControls.end(), ctrl);
    bool removed = it != foregroundControls.end();
    foregroundControls.erase(it, foregroundControls.end());
    if (removed) {
        if (auto p = pressedControl.lock()) {
            if (p == ctrl)
                pressedControl.reset();
        }
        markDirty();
    }
    return removed;
}

void WindowPanel::ClearAllControls() {
    backgroundControls.clear();
    midgroundControls.clear();
    foregroundControls.clear();
    pressedControl.reset();
    markDirty();
}

std::vector<std::shared_ptr<Controls>> WindowPanel::GetAllControls() const {
    return getAllControlsCached();
}

void WindowPanel::SetBackground(const ImageHandle &img) { background = img; }

void WindowPanel::ClearBackground() { background = {}; }

ImageHandle *WindowPanel::GetBackground() const {
    return background ? const_cast<ImageHandle *>(&background) : nullptr;
}

Controls *WindowPanel::FindTopControlAt(int x, int y) {
    const auto &controls = getAllControlsCached();
    for (auto it = controls.rbegin(); it != controls.rend(); ++it) {
        const auto &c = *it;
        if (c && c->IsVisible() && c->IsEnabled() && c->Contains(x, y)) {
            return c.get();
        }
    }
    return nullptr;
}

void WindowPanel::HandleEvent(const PlatformEvent &e) {
    if (auto *mb = std::get_if<MouseButtonEvent>(&e)) {
        if (mb->button == MouseButtonEvent::Left &&
            mb->action == MouseButtonEvent::Down) {
            HandleMouseDown(mb->x, mb->y);
        } else if (mb->button == MouseButtonEvent::Left &&
                   mb->action == MouseButtonEvent::Up) {
            HandleMouseUp(mb->x, mb->y);
        } else if (mb->button == MouseButtonEvent::Right &&
                   mb->action == MouseButtonEvent::Up) {
            if (OnMouseRBUPHook)
                OnMouseRBUPHook();
        }
        return;
    }

    if (auto *k = std::get_if<KeyEvent>(&e)) {
        if (k->action == KeyEvent::Down) {
            if (OnKeyDownHook)
                OnKeyDownHook(k->key, k->mods);
        } else {
            if (OnKeyUpHook)
                OnKeyUpHook(k->key, k->mods);
        }
        return;
    }

    if (auto *mm = std::get_if<MouseMoveEvent>(&e)) {
        if (OnMouseMoveHook)
            OnMouseMoveHook(mm->x, mm->y);
        return;
    }
}

void WindowPanel::SetOnMouseMoveHook(std::function<void(int, int)> hook) {
    OnMouseMoveHook = std::move(hook);
}

void WindowPanel::SetOnKeyDownHook(std::function<void(int, int)> hook) {
    OnKeyDownHook = std::move(hook);
}

void WindowPanel::SetOnKeyUpHook(std::function<void(int, int)> hook) {
    OnKeyUpHook = std::move(hook);
}

void WindowPanel::SetOnMouseRBUPHook(std::function<void()> hook) {
    OnMouseRBUPHook = std::move(hook);
}

void WindowPanel::HandleMouseDown(int x, int y) {
    Controls *top = FindTopControlAt(x, y);
    if (!top) {
        pressedControl.reset();
        return;
    }

    const auto &controls = getAllControlsCached();
    for (const auto &c : controls) {
        if (c.get() == top) {
            pressedControl = c;
            c->OnMouseDown(x, y);
            break;
        }
    }
}

void WindowPanel::HandleMouseUp(int x, int y) {
    auto pressed = pressedControl.lock();
    pressedControl.reset();

    if (!pressed)
        return;

    if (pressed->Contains(x, y)) {
        pressed->OnMouseUp(x, y);
    } else {
        pressed->OnMouseReset();
    }
}


