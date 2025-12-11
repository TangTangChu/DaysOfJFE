#pragma once
#include <memory>

#include "app/EventManager.h"
#include "audio/AudioService.h"

class WindowPanel;
class WindowManager;

class ApplicationContext {
  public:
    explicit ApplicationContext(WindowManager *manager);

    void RequestWindowSwitch(int index);
    void RequestWindowSwitch(std::shared_ptr<WindowPanel> window);
    std::shared_ptr<WindowPanel> RequestGetWindow(int index);

    AudioService audio; // 统一音频服务
    EventManager eventManager;

  private:
    WindowManager *m_manager = nullptr;
};
