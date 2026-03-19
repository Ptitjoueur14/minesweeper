#include <chrono>
#include "../include/timer.hpp"
#include "../include/ui.hpp"

bool Timer::updateTimer()
{
    if (GameUI::nbClicks == 0 || GameUI::isGameFinished)
    {
        return false;
    }

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    int newElapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - GameUI::gameStartTime).count();

    if (newElapsedSeconds != GameUI::elapsedSeconds)
    {
        GameUI::elapsedSeconds = newElapsedSeconds;
        return true;
    }
    return false;
}
