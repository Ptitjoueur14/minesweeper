#include <chrono>
#include "../include/timer.hpp"

bool Timer::isTimerActive = false;
std::chrono::steady_clock::time_point Timer::gameStartTime;
std::chrono::steady_clock::time_point Timer::gameEndTime;
int Timer::elapsedSeconds = 0;
int Timer::finalTimeSeconds = 0;
int Timer::finalTimeMilliseconds = 0;

void Timer::startTimer()
{
    if (isTimerActive)
    {
        return;
    }

    isTimerActive = true;
    gameStartTime = getTimeNow();
}

std::chrono::steady_clock::time_point Timer::getTimeNow()
{
    return std::chrono::steady_clock::now();
}

bool Timer::updateTimer()
{
    if (!isTimerActive)
    {
        return false;
    }

    std::chrono::steady_clock::time_point now = getTimeNow();
    int newElapsedSeconds = getElapsedTimeSeconds(gameStartTime, now);

    if (newElapsedSeconds != elapsedSeconds)
    {
        elapsedSeconds = newElapsedSeconds;
        return true;
    }
    
    return false;
}

void Timer::endTimer()
{
    if (!isTimerActive)
    {
        return;
    }

    gameEndTime = getTimeNow();
    isTimerActive = false;
    finalTimeSeconds = getElapsedTimeSeconds(gameStartTime, gameEndTime);
    finalTimeMilliseconds = getElapsedTimeMilliseconds(gameStartTime, gameEndTime);
}

// Gets the "Seconds" component of the time
int Timer::getElapsedTimeSeconds(std::chrono::steady_clock::time_point startTime, std::chrono::steady_clock::time_point endTime)
{
    return std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
}

// Gets the "Milliseconds" component of the time
int Timer::getElapsedTimeMilliseconds(std::chrono::steady_clock::time_point startTime, std::chrono::steady_clock::time_point endTime)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() % 1000;
}

void Timer::resetTimer()
{
    isTimerActive = false;
    elapsedSeconds = 0;
    finalTimeSeconds = 0;
}
