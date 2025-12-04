#pragma once
#include <memory>

#include "audio/MusicPlayer.h"
#include "app/EventManager.h"

class WindowPanel;
class WindowManager;

class ApplicationContext {
  public:
    explicit ApplicationContext(WindowManager *manager);

    void RequestWindowSwitch(int index);
    void RequestWindowSwitch(std::shared_ptr<WindowPanel> window);
    std::shared_ptr<WindowPanel> RequestGetWindow(int index);

    MusicPlayer bgm;
    MusicPlayer voice;
    MusicPlayer sfx;
    EventManager eventManager;

  private:
    WindowManager *m_manager = nullptr;
};
