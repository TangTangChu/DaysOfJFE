#pragma once
#include <memory>

#include "audio/MusicPlayer.h"

class WindowPanel;
class WindowManager;

class IGlobalEvent {
  public:
    explicit IGlobalEvent(WindowManager *manager);

    void RequestWindowSwitch(int index);
    void RequestWindowSwitch(std::shared_ptr<WindowPanel> window);
    std::shared_ptr<WindowPanel> RequestGetWindow(int index);

    MusicPlayer bgm;
    MusicPlayer voice;
    MusicPlayer sfx;

  private:
    WindowManager *m_manager = nullptr;
};
