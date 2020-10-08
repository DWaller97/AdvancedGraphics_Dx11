#pragma once

#include <Windows.h>

class Time
{

    private:
        
        Time* instance;
        static float deltaTime;
        static double totalTime;
    public:    
        Time();
        ~Time();
        void UpdateDeltaTime();
        static float GetDeltaTime() { return deltaTime; };
        double GetTotalTime() { return totalTime; };
};

