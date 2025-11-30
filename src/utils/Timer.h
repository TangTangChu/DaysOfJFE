#pragma once
#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

class Timer {
    std::atomic<bool> active{false};
    std::thread worker;

  public:
    Timer() = default;

    void start(int intervalMs, std::function<void()> callback) {
        if (active) {
            stop();
        }

        active = true;
        worker =
            std::thread([this, intervalMs, callback = std::move(callback)]() {
                while (active.load(std::memory_order_relaxed)) {
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(intervalMs));

                    if (active.load(std::memory_order_relaxed)) {
                        callback();
                    }
                }
            });
    }
    void stop() {
        active.store(false, std::memory_order_relaxed);
        if (worker.joinable()) {
            worker.join();
        }
    }

    ~Timer() { stop(); }

    Timer(const Timer &) = delete;
    Timer &operator=(const Timer &) = delete;
};
