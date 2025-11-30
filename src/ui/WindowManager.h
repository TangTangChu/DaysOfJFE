#pragma once
#include "app/IGlobalEvent.h"
#include "gfx/IRenderer.h"
#include "platform/PlatformEvent.h"
#include "ui/Controls.h"
#include "ui/WindowPanel.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

class WindowManager {
  public:
    enum class Layer { BACKGROUND, MIDGROUND, FOREGROUND };

  private:
    std::vector<std::shared_ptr<WindowPanel>> windows;
    std::shared_ptr<WindowPanel> currentWindow;
    IGlobalEvent *globalEvent = nullptr;

  public:
    WindowManager() = default;
    ~WindowManager() = default;

    void SetGlobalEvent(IGlobalEvent *g);

    void AddWindow(std::shared_ptr<WindowPanel> panel);
    void SwitchWindow(int index);
    std::shared_ptr<WindowPanel> GetCurrentWindow() const;

    bool AddControl(std::shared_ptr<Controls> ctrl,
                    Layer layer = Layer::MIDGROUND);
    bool RemoveControl(const std::shared_ptr<Controls> &ctrl);
    void ClearAllControls();

    void SetBackground(const ImageHandle &img);
    void ClearBackground();

    void Redraw(IRenderer &r);
    void Dispatch(const PlatformEvent &e);

    void ShowDialog(std::wstring text, std::function<void()> yes,
                    std::function<void()> no);
    void CloseDialog();
    void SwitchWindow(std::shared_ptr<WindowPanel> window);
    std::shared_ptr<WindowPanel> GetWindow(int index) const;
};
