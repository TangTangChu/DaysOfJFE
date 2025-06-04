#pragma once
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <functional>
#include <vector>
#include <any>
#include <memory>
#include "MusicPlayer.h"
class WindowPanel;
class WindowManager;

class IGlobalEvent {
public:
	explicit IGlobalEvent(WindowManager* manager);

	// �ṩ��WindowPanel�Ĳ�������
	void RequestWindowSwitch(int index);
	void RequestWindowSwitch(std::shared_ptr<WindowPanel> window);
	std::shared_ptr<WindowPanel> RequestGetWindow(int index);
	MusicPlayer bgm;
	MusicPlayer voice;
	MusicPlayer sfx;
private:
	WindowManager* m_manager;
};
