#pragma once
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>

/**
 * @file Timer.h
 * @brief 定时器类 - 支持周期性回调
 */
class Timer {
    std::atomic<bool> active{ false };
    std::thread worker;

public:
    void Start(int intervalMs, std::function<void()> callback) {
        active = true;
        worker = std::thread([=]() {
            while (active) {
                std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
                if (active) callback();
            }
        });
    }

    void Stop() {
        active = false;
        if (worker.joinable()) worker.join();
    }

    ~Timer() {
        Stop();
    }
};
