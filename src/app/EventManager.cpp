#include "app/EventManager.h"
#include "windows/WindowManager.h"
#include "windows/WindowPanel.h"
#include "ui/Controls.h"
#include <algorithm>

int EventManager::RegisterEventHandler(size_t eventTypeIndex,
                                       EventCallback callback,
                                       EventPriority priority) {
    int handlerId = m_nextHandlerId++;
    m_eventHandlers[eventTypeIndex].push_back(
        {std::move(callback), priority, handlerId});

    auto &handlers = m_eventHandlers[eventTypeIndex];
    std::sort(handlers.begin(), handlers.end(),
              [](const EventHandler &a, const EventHandler &b) {
                  return static_cast<int>(a.priority) <
                         static_cast<int>(b.priority);
              });

    return handlerId;
}

bool EventManager::UnregisterEventHandler(size_t eventTypeIndex,
                                          int handlerId) {
    auto it = m_eventHandlers.find(eventTypeIndex);
    if (it == m_eventHandlers.end())
        return false;

    auto &handlers = it->second;
    auto handlerIt = std::remove_if(
        handlers.begin(), handlers.end(),
        [handlerId](const EventHandler &h) { return h.id == handlerId; });

    bool removed = handlerIt != handlers.end();
    handlers.erase(handlerIt, handlers.end());

    if (handlers.empty()) {
        m_eventHandlers.erase(it);
    }

    return removed;
}

int EventManager::AddEventFilter(EventFilter filter) {
    int filterId = m_nextFilterId++;
    m_eventFilters.push_back({std::move(filter), filterId});
    return filterId;
}

bool EventManager::RemoveEventFilter(int filterId) {
    auto it = std::remove_if(m_eventFilters.begin(), m_eventFilters.end(),
                             [filterId](const FilterEntry &entry) {
                                 return entry.id == filterId;
                             });
    
    bool removed = it != m_eventFilters.end();
    m_eventFilters.erase(it, m_eventFilters.end());
    return removed;
}

void EventManager::ProcessEvent(const PlatformEvent &event) {
    for (const auto &entry : m_eventFilters) {
        if (entry.filter && !entry.filter(event)) {
            return; 
        }
    }

    switch (GetEventTypeIndex(event)) {
    case 0: // MouseButtonEvent
        ProcessMouseButtonEvent(std::get<MouseButtonEvent>(event));
        break;
    case 1: // MouseMoveEvent
        ProcessMouseMoveEvent(std::get<MouseMoveEvent>(event));
        break;
    case 2: // MouseWheelEvent
        ProcessMouseWheelEvent(std::get<MouseWheelEvent>(event));
        break;
    case 3: // KeyEvent
        ProcessKeyEvent(std::get<KeyEvent>(event));
        break;
    case 4: // TextInputEvent
        ProcessTextInputEvent(std::get<TextInputEvent>(event));
        break;
    case 5: // WindowResizeEvent
        ProcessWindowResizeEvent(std::get<WindowResizeEvent>(event));
        break;
    case 6: // FocusEvent
        ProcessFocusEvent(std::get<FocusEvent>(event));
        break;
    case 7: // TimerEvent
        if (auto timerEvent = std::get_if<TimerEvent>(&event)) {
            auto timerIt = m_timers.find(timerEvent->timerId);
            if (timerIt != m_timers.end()) {
                timerIt->second.callback();
            }
        }
        break;
    case 8: // GamepadEvent
        break;
    }
    FireEventToHandlers(GetEventTypeIndex(event), event);
}

void EventManager::ProcessMouseButtonEvent(const MouseButtonEvent &event) {
    switch (event.button) {
    case MouseButtonEvent::Left:
        m_mouseState.leftPressed = (event.action == MouseButtonEvent::Down);
        break;
    case MouseButtonEvent::Right:
        m_mouseState.rightPressed = (event.action == MouseButtonEvent::Down);
        break;
    case MouseButtonEvent::Middle:
        m_mouseState.middlePressed = (event.action == MouseButtonEvent::Down);
        break;
    }

    m_mouseState.x = event.x;
    m_mouseState.y = event.y;

    if (m_windowManager) {
        m_windowManager->Dispatch(event);
    }
    UpdateMouseHover(event.x, event.y);
}

void EventManager::ProcessMouseMoveEvent(const MouseMoveEvent &event) {
    m_mouseState.x = event.x;
    m_mouseState.y = event.y;
    if (m_windowManager) {
        m_windowManager->Dispatch(event);
    }
    UpdateMouseHover(event.x, event.y);
}

void EventManager::ProcessMouseWheelEvent(const MouseWheelEvent &event) {
    if (m_windowManager) {
        m_windowManager->Dispatch(event);
    }
}

void EventManager::ProcessKeyEvent(const KeyEvent &event) {
    m_keyStates[event.key] = (event.action != KeyEvent::Up);
    if (m_windowManager) {
        m_windowManager->Dispatch(event);
    }
}

void EventManager::ProcessTextInputEvent(const TextInputEvent &event) {
    // 文本输入事件处理
}

void EventManager::ProcessWindowResizeEvent(const WindowResizeEvent &event) {
    // 窗口大小改变事件处理
}

void EventManager::ProcessFocusEvent(const FocusEvent &event) {
    if (!event.focused) {
        // 失去焦点时重置输入状态
        m_mouseState.leftPressed = false;
        m_mouseState.rightPressed = false;
        m_mouseState.middlePressed = false;
        m_keyStates.clear();
    }
}

void EventManager::UpdateMouseHover(int x, int y) {
    if (!m_windowManager)
        return;

    auto currentWindow = m_windowManager->GetCurrentWindow();
    if (!currentWindow)
        return;

    Controls *newHovered = currentWindow->FindTopControlAt(x, y);

    if (newHovered != m_mouseState.hoveredControl) {
        // 触发离开事件给之前的悬停控件
        if (m_mouseState.hoveredControl) {
            m_mouseState.hoveredControl->OnMouseLeave(x, y);
        }
        
        // 触发进入事件给新的悬停控件
        if (newHovered) {
            newHovered->OnMouseEnter(x, y);
        }
        
        m_mouseState.hoveredControl = newHovered;
    }
}

void EventManager::FireEventToHandlers(size_t eventTypeIndex,
                                       const PlatformEvent &event) {
    auto it = m_eventHandlers.find(eventTypeIndex);
    if (it == m_eventHandlers.end())
        return;

    for (const auto &handler : it->second) {
        handler.callback(event);
    }
}

size_t EventManager::GetEventTypeIndex(const PlatformEvent &event) {
    return event.index();
}

int EventManager::CreateTimer(float intervalSeconds,
                              std::function<void()> callback, bool repeating) {
    int timerId = m_nextTimerId++;
    m_timers[timerId] = {timerId, intervalSeconds, 0.0f, repeating,
                         std::move(callback)};
    return timerId;
}

void EventManager::DestroyTimer(int timerId) { m_timers.erase(timerId); }

void EventManager::UpdateTimers(float deltaTime) {
    std::vector<int> timersToFire;

    for (auto &[timerId, timerInfo] : m_timers) {
        timerInfo.elapsed += deltaTime;
        if (timerInfo.elapsed >= timerInfo.interval) {
            timersToFire.push_back(timerId);
            timerInfo.elapsed = 0.0f;

            if (!timerInfo.repeating) {
                m_timers.erase(timerId);
            }
        }
    }

    for (int timerId : timersToFire) {
        auto it = m_timers.find(timerId);
        if (it != m_timers.end()) {
            it->second.callback();
        }
    }
}

bool EventManager::IsKeyPressed(int key) const {
    auto it = m_keyStates.find(key);
    return it != m_keyStates.end() && it->second;
}

bool EventManager::IsMouseButtonPressed(MouseButtonEvent::Button button) const {
    switch (button) {
    case MouseButtonEvent::Left:
        return m_mouseState.leftPressed;
    case MouseButtonEvent::Right:
        return m_mouseState.rightPressed;
    case MouseButtonEvent::Middle:
        return m_mouseState.middlePressed;
    default:
        return false;
    }
}

void EventManager::GetMousePosition(int &x, int &y) const {
    x = m_mouseState.x;
    y = m_mouseState.y;
}

void EventManager::SimulateMouseClick(int x, int y,
                                      MouseButtonEvent::Button button) {
    MouseButtonEvent downEvent;
    downEvent.x = x;
    downEvent.y = y;
    downEvent.button = button;
    downEvent.action = MouseButtonEvent::Down;
    ProcessEvent(downEvent);

    MouseButtonEvent upEvent;
    upEvent.x = x;
    upEvent.y = y;
    upEvent.button = button;
    upEvent.action = MouseButtonEvent::Up;
    ProcessEvent(upEvent);
}

void EventManager::SimulateKeyPress(int key, int mods) {
    KeyEvent downEvent;
    downEvent.key = key;
    downEvent.mods = mods;
    downEvent.action = KeyEvent::Down;
    ProcessEvent(downEvent);

    KeyEvent upEvent;
    upEvent.key = key;
    upEvent.mods = mods;
    upEvent.action = KeyEvent::Up;
    ProcessEvent(upEvent);
}
