#pragma once
#include "WindowPanel.h"
#include "ui/Layout.h"

class HomeWindow : public WindowPanel {
  public:
    HomeWindow();
    virtual ~HomeWindow() = default;

    void OnWindowResize(int width, int height) override;

  private:
    std::shared_ptr<StackPanel> buttonStack;
};
