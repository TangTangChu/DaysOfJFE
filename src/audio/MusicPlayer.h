#pragma once
#include <algorithm>
#include <atomic>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <AL/al.h>
#include <AL/alc.h>

class MusicPlayer {
  public:
    enum AudioType { BGM, VOICE, SFX };

    explicit MusicPlayer(AudioType type = BGM);
    ~MusicPlayer();

    bool Load(const std::string &filenameUtf8);
    bool Play(bool loop = false);
    void Pause();
    void Stop();
    void SetVolume(float volume);
    bool IsPlaying() const;

    void SetType(AudioType type);
    void FadeIn(float durationSec);
    void FadeOut(float durationSec);

  private:
    static std::atomic<bool> alInitialized;
    static ALCdevice *device;
    static ALCcontext *context;
    static std::mutex alInitMutex;
    static void InitializeOpenAL();

    AudioType currentType;
    float currentVolume = 0.3f;

    ALuint source = 0;
    ALuint buffer = 0;

    std::thread fadeThread;
    std::atomic<bool> fading{false};

    bool loadWav(const std::string &path, std::vector<int16_t> &pcm,
                 int &channels, int &sampleRate);
    bool loadMp3(const std::string &path, std::vector<int16_t> &pcm,
                 int &channels, int &sampleRate);
    bool loadFlac(const std::string &path, std::vector<int16_t> &pcm,
                  int &channels, int &sampleRate);

    bool uploadBuffer(const std::vector<int16_t> &pcm, int channels,
                      int sampleRate);
};
