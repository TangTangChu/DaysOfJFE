#pragma once
#include <memory>

// 前置声明
class WindowPanel;
class WindowManager;

/**
 * @file IGlobalEvent.h
 * @brief 全局事件接口 - 窗口切换和音频管理
 * 
 * 迁移说明:
 * - 音乐播放器暂时注释，等MusicPlayer迁移后再启用
 */
class IGlobalEvent {
public:
    explicit IGlobalEvent(WindowManager* manager);

    // 窗口切换请求
    void RequestWindowSwitch(int index);
    void RequestWindowSwitch(std::shared_ptr<WindowPanel> window);
    std::shared_ptr<WindowPanel> RequestGetWindow(int index);

    // 音乐播放器（待迁移）
    // MusicPlayer bgm;
    // MusicPlayer voice;
    // MusicPlayer sfx;

private:
    WindowManager* m_manager;
};
