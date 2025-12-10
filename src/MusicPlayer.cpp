#include "MusicPlayer.h"
#include <iostream>

bool MusicPlayer::mixerInitialized = false;

void MusicPlayer::InitializeMixer() {
    if (!mixerInitialized) {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            std::cerr << "SDL_mixer 初始化失败: " << Mix_GetError() << std::endl;
            mixerInitialized = false;
        } else {
            std::cout << "SDL_mixer 初始化成功" << std::endl;
            mixerInitialized = true;
            
            // 分配混音通道
            Mix_AllocateChannels(16);
        }
    }
}

MusicPlayer::MusicPlayer(AudioType type) : currentType(type) {
    InitializeMixer();
    isInitialized = mixerInitialized;
}

MusicPlayer::~MusicPlayer() {
    Stop();
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    if (sound) {
        Mix_FreeChunk(sound);
        sound = nullptr;
    }
}

bool MusicPlayer::Load(const std::string& filename) {
    if (!isInitialized) {
        std::cerr << "MusicPlayer 未初始化" << std::endl;
        return false;
    }

    // 释放旧资源
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    if (sound) {
        Mix_FreeChunk(sound);
        sound = nullptr;
    }

    if (currentType == BGM) {
        // BGM 使用 Mix_Music
        music = Mix_LoadMUS(filename.c_str());
        if (!music) {
            std::cerr << "加载音乐失败: " << filename 
                      << " (错误: " << Mix_GetError() << ")" << std::endl;
            return false;
        }
    } else {
        // VOICE 和 SFX 使用 Mix_Chunk
        sound = Mix_LoadWAV(filename.c_str());
        if (!sound) {
            std::cerr << "加载音效失败: " << filename 
                      << " (错误: " << Mix_GetError() << ")" << std::endl;
            return false;
        }
    }

    SetVolume(currentVolume);
    return true;
}

bool MusicPlayer::Play(bool loop) {
    if (!isInitialized) return false;

    if (currentType == BGM && music) {
        // BGM 循环播放
        int loops = loop ? -1 : 0;
        if (Mix_PlayMusic(music, loops) < 0) {
            std::cerr << "播放音乐失败: " << Mix_GetError() << std::endl;
            return false;
        }
        return true;
    } else if (sound) {
        // 音效播放
        int loops = loop ? -1 : 0;
        channelId = Mix_PlayChannel(-1, sound, loops);
        if (channelId < 0) {
            std::cerr << "播放音效失败: " << Mix_GetError() << std::endl;
            return false;
        }
        return true;
    }

    return false;
}

void MusicPlayer::Pause() {
    if (!isInitialized) return;

    if (currentType == BGM) {
        Mix_PauseMusic();
    } else if (channelId >= 0) {
        Mix_Pause(channelId);
    }
}

void MusicPlayer::Stop() {
    if (!isInitialized) return;

    if (currentType == BGM) {
        Mix_HaltMusic();
    } else if (channelId >= 0) {
        Mix_HaltChannel(channelId);
        channelId = -1;
    }
}

void MusicPlayer::SetVolume(float volume) {
    currentVolume = std::max(0.0f, std::min(1.0f, volume));
    
    if (!isInitialized) return;

    if (currentType == BGM) {
        Mix_VolumeMusic(static_cast<int>(currentVolume * MIX_MAX_VOLUME));
    } else if (sound) {
        Mix_VolumeChunk(sound, static_cast<int>(currentVolume * MIX_MAX_VOLUME));
    }
}

bool MusicPlayer::IsPlaying() {
    if (!isInitialized) return false;

    if (currentType == BGM) {
        return Mix_PlayingMusic() != 0;
    } else if (channelId >= 0) {
        return Mix_Playing(channelId) != 0;
    }

    return false;
}

void MusicPlayer::SetType(AudioType type) {
    currentType = type;
}

void MusicPlayer::FadeIn(float duration) {
    if (!isInitialized) return;

    int ms = static_cast<int>(duration * 1000);
    
    if (currentType == BGM && music) {
        Mix_FadeInMusic(music, -1, ms);
    } else if (sound && channelId < 0) {
        channelId = Mix_FadeInChannel(-1, sound, 0, ms);
    }
}

void MusicPlayer::FadeOut(float duration) {
    if (!isInitialized) return;

    int ms = static_cast<int>(duration * 1000);
    
    if (currentType == BGM) {
        Mix_FadeOutMusic(ms);
    } else if (channelId >= 0) {
        Mix_FadeOutChannel(channelId, ms);
    }
}
