#include "app/IGlobalEvent.h"
#include "windows/WindowManager.h"

IGlobalEvent::IGlobalEvent(WindowManager *manager)
    : bgm(MusicPlayer::BGM), voice(MusicPlayer::VOICE), sfx(MusicPlayer::SFX),
      m_manager(manager) {}

void IGlobalEvent::RequestWindowSwitch(int index) {
    if (m_manager)
        m_manager->SwitchWindow(index);
}

void IGlobalEvent::RequestWindowSwitch(std::shared_ptr<WindowPanel> window) {
    if (m_manager)
        m_manager->SwitchWindow(std::move(window));
}

std::shared_ptr<WindowPanel> IGlobalEvent::RequestGetWindow(int index) {
    return m_manager ? m_manager->GetWindow(index) : nullptr;
}
