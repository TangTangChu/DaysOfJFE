#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
class Timer {
    std::atomic<bool> active{ false };
    std::thread worker;

public:
    void start(int intervalMs, std::function<void()> callback) {
        active = true;
        worker = std::thread([=]() {
            while (active) {
                std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
                if (active) callback();
            }
            });
    }

    void stop() {
        active = false;
        if (worker.joinable()) worker.join();
    }
};