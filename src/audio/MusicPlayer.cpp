#include "MusicPlayer.h"
#include <chrono>
#include <filesystem>

#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>

#define DR_FLAC_IMPLEMENTATION
#include <dr_flac.h>

std::atomic<bool> MusicPlayer::alInitialized{false};
ALCdevice *MusicPlayer::device = nullptr;
ALCcontext *MusicPlayer::context = nullptr;
std::mutex MusicPlayer::alInitMutex;

static std::string ToLowerExt(const std::string &p) {
    auto ext = std::filesystem::path(p).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

void MusicPlayer::InitializeOpenAL() {
    std::lock_guard<std::mutex> lock(alInitMutex);
    if (alInitialized)
        return;

    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "OpenAL device open failed\n";
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!context || !alcMakeContextCurrent(context)) {
        std::cerr << "OpenAL context create failed\n";
        if (context)
            alcDestroyContext(context);
        alcCloseDevice(device);
        context = nullptr;
        device = nullptr;
        return;
    }

    alInitialized = true;
}

MusicPlayer::MusicPlayer(AudioType type) : currentType(type) {
    InitializeOpenAL();
    if (!alInitialized)
        return;

    alGenSources(1, &source);
    alSourcef(source, AL_GAIN, currentVolume);
}

MusicPlayer::~MusicPlayer() {
    fading = false;
    if (fadeThread.joinable())
        fadeThread.join();

    Stop();
    if (source)
        alDeleteSources(1, &source);
    if (buffer)
        alDeleteBuffers(1, &buffer);
}

bool MusicPlayer::Load(const std::string &filenameUtf8) {
    if (!alInitialized)
        return false;

    Stop();
    if (buffer) {
        alDeleteBuffers(1, &buffer);
        buffer = 0;
    }

    std::vector<int16_t> pcm;
    int channels = 0, sampleRate = 0;

    const auto ext = ToLowerExt(filenameUtf8);
    bool ok = false;
    if (ext == ".wav")
        ok = loadWav(filenameUtf8, pcm, channels, sampleRate);
    else if (ext == ".mp3")
        ok = loadMp3(filenameUtf8, pcm, channels, sampleRate);
    else if (ext == ".flac")
        ok = loadFlac(filenameUtf8, pcm, channels, sampleRate);
    else {
        std::cerr << "Unsupported audio format: " << filenameUtf8 << "\n";
        return false;
    }

    if (!ok)
        return false;
    return uploadBuffer(pcm, channels, sampleRate);
}

bool MusicPlayer::uploadBuffer(const std::vector<int16_t> &pcm, int channels,
                               int sampleRate) {
    ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, pcm.data(),
                 (ALsizei)(pcm.size() * sizeof(int16_t)), sampleRate);

    alSourcei(source, AL_BUFFER, buffer);
    alSourcef(source, AL_GAIN, currentVolume);
    return true;
}

bool MusicPlayer::Play(bool loop) {
    if (!source || !buffer)
        return false;

    if (currentType == VOICE)
        loop = false;

    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcePlay(source);
    return true;
}

void MusicPlayer::Pause() {
    if (source)
        alSourcePause(source);
}

void MusicPlayer::Stop() {
    if (!source)
        return;
    alSourceStop(source);
    alSourcei(source, AL_BUFFER, 0);
}

void MusicPlayer::SetVolume(float volume) {
    currentVolume = std::clamp(volume, 0.0f, 1.0f);
    if (source)
        alSourcef(source, AL_GAIN, currentVolume);
}

bool MusicPlayer::IsPlaying() const {
    if (!source)
        return false;
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

void MusicPlayer::SetType(AudioType type) { currentType = type; }

void MusicPlayer::FadeIn(float durationSec) {
    if (!source)
        return;
    fading = false;
    if (fadeThread.joinable())
        fadeThread.join();
    fading = true;

    const float target = currentVolume;
    alSourcef(source, AL_GAIN, 0.0f);
    Play(currentType == BGM);

    fadeThread = std::thread([this, durationSec, target]() {
        auto start = std::chrono::steady_clock::now();
        while (fading) {
            auto elapsed = std::chrono::duration<float>(
                               std::chrono::steady_clock::now() - start)
                               .count();
            float t = std::clamp(elapsed / durationSec, 0.0f, 1.0f);
            alSourcef(source, AL_GAIN, target * t);
            if (t >= 1.0f)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        fading = false;
    });
}

void MusicPlayer::FadeOut(float durationSec) {
    if (!source)
        return;
    fading = false;
    if (fadeThread.joinable())
        fadeThread.join();
    fading = true;

    float startVol;
    alGetSourcef(source, AL_GAIN, &startVol);

    fadeThread = std::thread([this, durationSec, startVol]() {
        auto start = std::chrono::steady_clock::now();
        while (fading) {
            auto elapsed = std::chrono::duration<float>(
                               std::chrono::steady_clock::now() - start)
                               .count();
            float t = std::clamp(elapsed / durationSec, 0.0f, 1.0f);
            alSourcef(source, AL_GAIN, startVol * (1.0f - t));
            if (t >= 1.0f)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        Stop();
        alSourcef(source, AL_GAIN, currentVolume);
        fading = false;
    });
}

bool MusicPlayer::loadWav(const std::string &path, std::vector<int16_t> &pcm,
                          int &channels, int &sampleRate) {
    drwav wav;
    if (!drwav_init_file(&wav, path.c_str(), nullptr)) {
        std::cerr << "WAV load failed: " << path << "\n";
        return false;
    }

    channels = wav.channels;
    sampleRate = wav.sampleRate;

    pcm.resize((size_t)wav.totalPCMFrameCount * channels);
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pcm.data());

    drwav_uninit(&wav);
    return true;
}

bool MusicPlayer::loadMp3(const std::string &path, std::vector<int16_t> &pcm,
                          int &channels, int &sampleRate) {
    drmp3 mp3;
    if (!drmp3_init_file(&mp3, path.c_str(), nullptr)) {
        std::cerr << "MP3 load failed: " << path << "\n";
        return false;
    }

    channels = mp3.channels;
    sampleRate = mp3.sampleRate;

    drmp3_uint64 frames = drmp3_get_pcm_frame_count(&mp3);
    pcm.resize((size_t)frames * channels);
    drmp3_read_pcm_frames_s16(&mp3, frames, pcm.data());

    drmp3_uninit(&mp3);
    return true;
}

bool MusicPlayer::loadFlac(const std::string &path, std::vector<int16_t> &pcm,
                           int &channels, int &sampleRate) {
    drflac *flac = drflac_open_file(path.c_str(), nullptr);
    if (!flac) {
        std::cerr << "FLAC load failed: " << path << "\n";
        return false;
    }

    channels = flac->channels;
    sampleRate = flac->sampleRate;

    drflac_uint64 frames = flac->totalPCMFrameCount;
    if (frames == 0) {
        std::cerr << "FLAC totalPCMFrameCount is 0 (unknown): " << path << "\n";
        drflac_close(flac);
        return false;
    }

    pcm.resize((size_t)frames * channels);
    drflac_read_pcm_frames_s16(flac, frames, pcm.data());

    drflac_close(flac);
    return true;
}
