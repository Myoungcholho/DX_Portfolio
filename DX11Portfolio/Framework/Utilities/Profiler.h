#pragma once

#include <Windows.h>
#include <iostream>
struct PROFILE_SAMPLE
{
	long			lFlag;
	WCHAR			szName[64];

	LARGE_INTEGER	lStartTime;

	__int64			iTotalTime;
	__int64			iMin;
	__int64			iMax;

	__int64			iCall;
};

void PRO_BEGIN(const WCHAR* s);
void PRO_END(const WCHAR* s);

class Profile
{
public:
	enum
	{
		PROFILE_MAX_NUM = 10,
		FILE_NAME_MAX = 64
	};
private:
	PROFILE_SAMPLE _profile[PROFILE_MAX_NUM];
	char _logFileName[FILE_NAME_MAX];
	LARGE_INTEGER baseTime;
	LARGE_INTEGER start;
public:
	Profile(const char* szLogFile = "ProFiling");
	~Profile();
private:
	void ProfileBegin(const WCHAR* szName);
	void ProfileEnd(const WCHAR* szName);
	void SaveLogFile();
	friend void PRO_BEGIN(const WCHAR* s);
	friend void PRO_END(const WCHAR* s);

public:
	void SetStartTime();					// 시작 시간을 임의로 바꾸고 싶다면

private:
	LARGE_INTEGER programStart;
	LARGE_INTEGER programEnd;
};

extern Profile g_profile;

#define PROFILE

#ifdef PROFILE
inline void PRO_BEGIN(const WCHAR* s)
{
	g_profile.ProfileBegin(s);
}

// 여기부터 하기
inline void PRO_END(const WCHAR* s)
{
	g_profile.ProfileEnd(s);
}

#else
inline void PRO_BEGIN(const WCHAR*) {}
inline void PRO_END(const WCHAR*) {}
#endif
