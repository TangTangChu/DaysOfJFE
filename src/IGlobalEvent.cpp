#include "IGlobalEvent.h"
#include "WindowManager.h"

IGlobalEvent::IGlobalEvent(WindowManager* manager) 
    : m_manager(manager) 
{
}

void IGlobalEvent::RequestWindowSwitch(int index) {
    if (m_manager) {
        m_manager->SwitchWindow(index);
    }
}

void IGlobalEvent::RequestWindowSwitch(std::shared_ptr<WindowPanel> window) {
    if (m_manager) {
        m_manager->SwitchWindow(window);
    }
}

std::shared_ptr<WindowPanel> IGlobalEvent::RequestGetWindow(int index) {
    if (m_manager) {
        return m_manager->GetWindow(index);
    }
    return nullptr;
}
