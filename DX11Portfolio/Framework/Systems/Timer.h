#pragma once

class CTimer
{
public:
	static void Create();
	static void Destroy();

	static CTimer* Get();

public:
	void InitWorldTime(float InValue) { WorldTime = InValue * 1000.0f; }

public:
	bool* GetAutoWorldTime() { return &bAutoWorldTime; }

public:
	float GetDeltaTime() { return DeltaTime; }
	float GetRunningTime() { return RunningTime; }

	float* GetManualTimeSpeed() { return &ManualTimeSpeed; }
	float* GetDayTimeSpeed() { return &DayTimeSpeed; }

	float GetLightAngle() { return LightAngle; }
	int GetHours() { return Hours; }
	int GetMinutes() { return Minutes; }

public:
	void AddWorldTime(float InValue);

public:
	void Tick();

private:
	static CTimer* Instance;

private:
	CTimer();
	~CTimer();

private:
	steady_clock::time_point StartTime;
	steady_clock::time_point PrevFrameTime;

	float DeltaTime = 0.0f;
	float RunningTime = 0.0f;

private:
	bool bAutoWorldTime = false;

	float ManualTimeSpeed = 250.0f;
	float DayTimeSpeed = 1000.0f;

	//1일 전체 초, 실제 계산시 밀리초로 변환
	float SecondsPerDay = 86400.0f;

	float WorldTime = 0.0f;
	float LightAngle = 0.0f;

	int Hours;
	int Minutes;
};