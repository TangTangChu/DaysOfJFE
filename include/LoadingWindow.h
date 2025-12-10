#pragma once
#include "WindowPanel.h"
#include "ImageContainer.h"
#include <functional>
#include <memory>
#include <vector>

class LoadingWindow : public WindowPanel {
private:
    std::function<void()> FinalHook;
    std::shared_ptr<ImageContainer> bgimage;
    int fadeInTime, fadeOutTime;
    std::vector<std::string> imgList;

public:
    LoadingWindow(std::vector<std::string> imglist = {}, int fadeIn = 700, int fadeOut = 700);
    void SetFinalHook(std::function<void()> fh);
    void StartAnimation();
};
