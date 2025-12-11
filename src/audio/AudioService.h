#pragma once

#include "audio/MusicPlayer.h"
#include <string>

class AudioService {
  public:
    AudioService();

    // ---- BGM ----
    void PlayBgm(const std::string &file, bool loop = true);
    void StopBgm();
    void PauseBgm();
    void SetBgmVolume(float volume);
    void FadeInBgm(const std::string &file, float durationSec);
    void FadeOutBgm(float durationSec);

    // ---- Voice ----
    void PlayVoice(const std::string &file); // 语音默认不循环
    void StopVoice();
    void PauseVoice();
    void SetVoiceVolume(float volume);

    // ---- SFX ----
    void PlaySfx(const std::string &file);   // 短音效一次性播放
    void StopSfx();                          // 停止当前 SFX
    void SetSfxVolume(float volume);

  private:
    MusicPlayer bgm_{MusicPlayer::BGM};
    MusicPlayer voice_{MusicPlayer::VOICE};
    MusicPlayer sfx_{MusicPlayer::SFX};
};
