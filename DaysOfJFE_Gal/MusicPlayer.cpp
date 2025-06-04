#include "MusicPlayer.h"

bool MusicPlayer::bassInitialized = false;

void MusicPlayer::InitializeBASS() {
    if (!bassInitialized) {
        bassInitialized = BASS_Init(-1, 44100, BASS_DEVICE_LATENCY, nullptr, nullptr);
        BASS_PluginLoad("bassflac.dll", 0);
        if (!bassInitialized) {
            std::cerr << "Failed to initialize BASS: " << BASS_ErrorGetCode() << std::endl;
        }
        else {

            BASS_SetConfig(BASS_CONFIG_DEV_BUFFER, 100);
            BASS_SetConfig(BASS_CONFIG_VISTA_TRUEPOS, 1);
        }
    }
}

MusicPlayer::MusicPlayer(AudioType type) : currentType(type) {
    InitializeBASS();
    isInitialized = bassInitialized;
}

MusicPlayer::~MusicPlayer() {
    Stop();
}

bool MusicPlayer::Load(const std::wstring& filename) {
    //std::cout << filename << std::endl;
    if (!isInitialized) return false;

    // 释放现有流
    if (musicStream) {
        BASS_StreamFree(musicStream);
        musicStream = 0;
    }

    // 根据类型设置不同标志
    DWORD flags = 0;
    switch (currentType) {
    case BGM:
        flags = BASS_SAMPLE_LOOP | BASS_SAMPLE_FLOAT;
        break;
    case VOICE:
        flags = BASS_SAMPLE_MONO;
        break;
    case SFX:
        flags = BASS_SAMPLE_3D;
        break;
    }

    musicStream = BASS_StreamCreateFile(FALSE, filename.c_str(), 0, 0, flags| BASS_UNICODE);
    if (!musicStream) {
        std::wcerr << "Failed to load audio: " << filename
            << " (Error: " << BASS_ErrorGetCode() << ")" << std::endl;
        return false;
    }

    SetVolume(currentVolume);  // 保持音量设置
    return true;
}

bool MusicPlayer::Play(bool loop) {
    if (!musicStream) return false;

    if (currentType == VOICE) loop = false;

    BASS_ChannelFlags(musicStream, loop ? BASS_SAMPLE_LOOP : 0, BASS_SAMPLE_LOOP);



    return BASS_ChannelPlay(musicStream, TRUE);
}

void MusicPlayer::Pause() {
    if (musicStream) BASS_ChannelPause(musicStream);
}

void MusicPlayer::Stop() {
    if (musicStream) BASS_ChannelStop(musicStream);
}

void MusicPlayer::SetVolume(float volume) {
    currentVolume = std::clamp(volume, 0.0f, 1.0f);
    if (musicStream) {
        BASS_ChannelSetAttribute(musicStream, BASS_ATTRIB_VOL, currentVolume);
    }
}

bool MusicPlayer::IsPlaying() {
    return musicStream && (BASS_ChannelIsActive(musicStream) == BASS_ACTIVE_PLAYING);
}

void MusicPlayer::SetType(AudioType type) {
    if (type != currentType) {
        Stop();
        currentType = type;
    }
}

void MusicPlayer::FadeIn(float duration) {
    if (!musicStream) return;
    BASS_ChannelSlideAttribute(musicStream, BASS_ATTRIB_VOL, currentVolume, duration * 1000);
    Play();
}

void MusicPlayer::FadeOut(float duration) {
    if (!musicStream) return;
    BASS_ChannelSlideAttribute(musicStream, BASS_ATTRIB_VOL, 0, duration * 1000);
}
