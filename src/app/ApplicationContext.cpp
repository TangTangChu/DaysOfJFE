#include "app/ApplicationContext.h"
#include "windows/WindowManager.h"

ApplicationContext::ApplicationContext(WindowManager *manager)
    : m_manager(manager) {
    eventManager.SetWindowManager(manager);
}

void ApplicationContext::RequestWindowSwitch(int index) {
    if (m_manager)
        m_manager->SwitchWindow(index);
}

void ApplicationContext::RequestWindowSwitch(
    std::shared_ptr<WindowPanel> window) {
    if (m_manager)
        m_manager->SwitchWindow(std::move(window));
}

std::shared_ptr<WindowPanel> ApplicationContext::RequestGetWindow(int index) {
    return m_manager ? m_manager->GetWindow(index) : nullptr;
}
