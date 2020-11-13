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

float Time::DeltaTime()
{
	static float t = 0.0f;
	static float prevTime = t;
	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;

	t = (dwTimeCur - dwTimeStart) / 1000.0f;
	float deltaTime = (t - prevTime);
	prevTime = t;
	return deltaTime;
}