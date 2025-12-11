#include "audio/AudioService.h"

AudioService::AudioService() = default;

// ---- BGM ----

void AudioService::PlayBgm(const std::string &file, bool loop) {
    if (bgm_.Load(file)) {
        bgm_.Play(loop);
    }
}

void AudioService::StopBgm() { bgm_.Stop(); }

void AudioService::PauseBgm() { bgm_.Pause(); }

void AudioService::SetBgmVolume(float volume) { bgm_.SetVolume(volume); }

void AudioService::FadeInBgm(const std::string &file, float durationSec) {
    if (bgm_.Load(file)) {
        bgm_.FadeIn(durationSec);
    }
}

void AudioService::FadeOutBgm(float durationSec) { bgm_.FadeOut(durationSec); }

// ---- Voice ----

void AudioService::PlayVoice(const std::string &file) {
    if (voice_.Load(file)) {
        voice_.Play(false); // 语音默认不循环
    }
}

void AudioService::StopVoice() { voice_.Stop(); }

void AudioService::PauseVoice() { voice_.Pause(); }

void AudioService::SetVoiceVolume(float volume) {
    voice_.SetVolume(volume);
}

// ---- SFX ----

void AudioService::PlaySfx(const std::string &file) {
    if (sfx_.Load(file)) {
        sfx_.Play(false);
    }
}

void AudioService::StopSfx() { sfx_.Stop(); }

void AudioService::SetSfxVolume(float volume) { sfx_.SetVolume(volume); }
