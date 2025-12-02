#pragma once
#include "ui/ImageContainer.h"
#include "windows/WindowPanel.h"
#include <functional>
#include <memory>
#include <vector>

class LoadingWindow : public WindowPanel {
  private:
    std::function<void()> FinalHook;
    int fadeInTime = 700;
    int fadeOutTime = 700;
    std::vector<ImageHandle> imgList;
    std::atomic<float> bgAlpha{1.0f};

  public:
    LoadingWindow(std::vector<ImageHandle> images = {}, int fadeIn = 700,
                  int fadeOut = 700);
    float GetBackgroundAlpha() const { return bgAlpha.load(); }
    void SetFinalHook(std::function<void()> fh);
    void StartAnimation();
    bool HasCustomBackground() const override { return true; }
    void DrawCustomBackground(Canvas &c, IRenderer &r) override;
};
