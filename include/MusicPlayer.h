#pragma once
#include <SDL2/SDL_mixer.h>
#include <string>

/**
 * @file MusicPlayer.h
 * @brief 音频播放器 - 支持BGM、语音、音效
 */
class MusicPlayer {
public:
    enum AudioType { BGM, VOICE, SFX };

    MusicPlayer(AudioType type = BGM);
    ~MusicPlayer();

    bool Load(const std::string& filename);
    bool Play(bool loop = false);
    void Pause();
    void Stop();
    void SetVolume(float volume);  // 0.0 到 1.0
    bool IsPlaying();

    void SetType(AudioType type);
    void FadeIn(float duration);
    void FadeOut(float duration);

private:
    static bool mixerInitialized;
    static void InitializeMixer();

    Mix_Music* music = nullptr;
    Mix_Chunk* sound = nullptr;
    AudioType currentType;
    float currentVolume = 0.3f;
    bool isInitialized = false;
    int channelId = -1;  // 用于音效通道
};
