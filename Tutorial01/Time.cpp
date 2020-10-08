#include "Time.h"

float Time::deltaTime = 0.01f;
double Time::totalTime = 0.0f;

Time::Time()
{
    instance = this;
}

Time::~Time()
{
    instance = nullptr;
}

void Time::UpdateDeltaTime()
{
    static float t = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if(timeStart == 0)
        timeStart = timeCur;
    t = (timeCur - timeStart) / 1000.0f;
    timeCur = timeStart;
    totalTime += t;
    deltaTime = 1.0f / (totalTime - t);
    if(deltaTime < 0.16f)
        deltaTime = 0.16f;

}