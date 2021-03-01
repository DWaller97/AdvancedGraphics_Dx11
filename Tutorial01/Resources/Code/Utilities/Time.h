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
        static float DeltaTime();
        double GetTotalTime() { return totalTime; };
};

