#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono>

class Timer
{
    public:
        static bool isTimerActive;
        static std::chrono::steady_clock::time_point gameStartTime;
        static std::chrono::steady_clock::time_point gameEndTime;
        static int elapsedSeconds;
        static double finalTimeSeconds;

        static void startTimer();
        static std::chrono::steady_clock::time_point getTimeNow();
        static bool updateTimer();
        static void endTimer();

        static int getElapsedTimeSeconds(std::chrono::steady_clock::time_point startTime, std::chrono::steady_clock::time_point endTime);
        static int getElapsedTimeMilliseconds(std::chrono::steady_clock::time_point startTime, std::chrono::steady_clock::time_point endTime);

        static void resetTimer();
};

#endif
