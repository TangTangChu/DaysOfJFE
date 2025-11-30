#pragma once
#include "ui/ImageContainer.h"
#include "ui/WindowPanel.h"
#include <functional>
#include <memory>
#include <vector>

class LoadingWindow : public WindowPanel {
  private:
    std::function<void()> FinalHook;
    int fadeInTime = 700;
    int fadeOutTime = 700;
    std::vector<ImageHandle> imgList;

  public:
    LoadingWindow(std::vector<ImageHandle> images = {}, int fadeIn = 700,
                  int fadeOut = 700);

    void SetFinalHook(std::function<void()> fh);
    void StartAnimation();
};
