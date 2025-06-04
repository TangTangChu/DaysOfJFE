#include "stdafx.h"
#include "IGlobalEvent.h"
#include "WindowManager.h"
#include "MusicPlayer.h"

IGlobalEvent::IGlobalEvent(WindowManager* manager) : m_manager(manager),
	bgm(MusicPlayer::BGM),voice(MusicPlayer::VOICE),sfx(MusicPlayer::SFX){}
void IGlobalEvent::RequestWindowSwitch(int index) {
	m_manager->SwitchWindow(index);
}
void IGlobalEvent::RequestWindowSwitch(std::shared_ptr<WindowPanel> window) {
	m_manager->SwitchWindow(window);
}
std::shared_ptr<WindowPanel> IGlobalEvent::RequestGetWindow(int index) {
	return m_manager->GetWindow(index);
}