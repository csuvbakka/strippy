#pragma once

#include <chrono>

class Timer
{
public:
    Timer(int milliseconds);
    volatile bool is_timed_out();
    void reset();

private:
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::milliseconds timeout_milliseconds_;
};
