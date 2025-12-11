#pragma once
#include "WindowPanel.h"
#include "ui/Layout.h"

class SettingWindow : public WindowPanel {
  public:
    SettingWindow(std::function<void()> backHook, const std::string &dialog);
    virtual ~SettingWindow() = default;

    void OnWindowResize(int width, int height) override;
    bool HasCustomBackground() const override { return true; }
    void DrawCustomBackground(Canvas &c, IRenderer &r) override;

  private:
    std::shared_ptr<StackPanel> contentStack;
    Color bgColor{0, 0, 0, 150}; // Semi-transparent overlay
};
