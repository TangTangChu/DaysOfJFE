#pragma once
#include "lib/bass/bass.h"
#include <string>
#include <iostream>
#include <algorithm>

class MusicPlayer {
public:
    enum AudioType { BGM, VOICE, SFX };

    MusicPlayer(AudioType type = BGM);
    ~MusicPlayer();


    bool Load(const std::wstring& filename);
    bool Play(bool loop = false);
    void Pause();
    void Stop();
    void SetVolume(float volume);
    bool IsPlaying();

    void SetType(AudioType type);  
    void FadeIn(float duration);   
    void FadeOut(float duration);  

private:
    static bool bassInitialized;
    static void InitializeBASS();

    HSTREAM musicStream = 0;
    AudioType currentType;
    float currentVolume = 0.3f;  
    bool isInitialized = false;
};
