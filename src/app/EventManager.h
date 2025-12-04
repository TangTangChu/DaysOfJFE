#pragma once
#include "platform/PlatformEvent.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

class WindowManager;
class Controls;

class EventManager {
  public:
    using EventCallback = std::function<void(const PlatformEvent &)>;
    using EventFilter = std::function<bool(const PlatformEvent &)>;

    enum class EventPriority {
        System,      // 系统级事件
        Application, // 应用级事件
        Window,      // 窗口级事件
        Control,     // 控件级事件
        Default = Application
    };

  private:
    struct EventHandler {
        EventCallback callback;
        EventPriority priority;
        int id;
    };

    struct FilterEntry {
        EventFilter filter;
        int id;
    };

    WindowManager *m_windowManager = nullptr;
    std::unordered_map<size_t, std::vector<EventHandler>> m_eventHandlers;
    std::vector<FilterEntry> m_eventFilters;
    int m_nextHandlerId = 1;
    int m_nextFilterId = 1;

    // 鼠标状态跟踪
    struct MouseState {
        int x = 0;
        int y = 0;
        bool leftPressed = false;
        bool rightPressed = false;
        bool middlePressed = false;
        Controls *hoveredControl = nullptr;
        Controls *pressedControl = nullptr;
    } m_mouseState;

    std::unordered_map<int, bool> m_keyStates;

    struct TimerInfo {
        int id;
        float interval; // 秒
        float elapsed;
        bool repeating;
        std::function<void()> callback;
    };
    std::unordered_map<int, TimerInfo> m_timers;
    int m_nextTimerId = 1;

  public:
    EventManager() = default;
    ~EventManager() = default;

    void SetWindowManager(WindowManager *wm) { m_windowManager = wm; }

    // 事件处理
    void ProcessEvent(const PlatformEvent &event);

    // 事件注册/注销
    int RegisterEventHandler(size_t eventTypeIndex, EventCallback callback,
                             EventPriority priority = EventPriority::Default);
    bool UnregisterEventHandler(size_t eventTypeIndex, int handlerId);

    // 事件过滤器
    int AddEventFilter(EventFilter filter);
    bool RemoveEventFilter(int filterId);

    // 定时器功能
    int CreateTimer(float intervalSeconds, std::function<void()> callback,
                    bool repeating = true);
    void DestroyTimer(int timerId);
    void UpdateTimers(float deltaTime);

    // 输入状态查询
    bool IsKeyPressed(int key) const;
    bool IsMouseButtonPressed(MouseButtonEvent::Button button) const;
    void GetMousePosition(int &x, int &y) const;
    Controls *GetHoveredControl() const { return m_mouseState.hoveredControl; }

    // 事件模拟
    void SimulateMouseClick(
        int x, int y, MouseButtonEvent::Button button = MouseButtonEvent::Left);
    void SimulateKeyPress(int key, int mods = 0);

  private:
    void ProcessMouseButtonEvent(const MouseButtonEvent &event);
    void ProcessMouseMoveEvent(const MouseMoveEvent &event);
    void ProcessMouseWheelEvent(const MouseWheelEvent &event);
    void ProcessKeyEvent(const KeyEvent &event);
    void ProcessTextInputEvent(const TextInputEvent &event);
    void ProcessWindowResizeEvent(const WindowResizeEvent &event);
    void ProcessFocusEvent(const FocusEvent &event);

    void UpdateMouseHover(int x, int y);
    void FireEventToHandlers(size_t eventTypeIndex, const PlatformEvent &event);

    static size_t GetEventTypeIndex(const PlatformEvent &event);
};
