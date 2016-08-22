#include <timer.hpp>
#include <iostream>

Timer::Timer(int milliseconds)
    : start_(std::chrono::high_resolution_clock::now())
    , timeout_milliseconds_(milliseconds)
{
}

volatile bool Timer::is_timed_out()
{
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start_);

    return duration > timeout_milliseconds_;
}

void Timer::reset()
{
    start_ = std::chrono::high_resolution_clock::now();
}
