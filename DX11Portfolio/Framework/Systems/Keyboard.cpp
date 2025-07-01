#include "Framework.h"
#include "Keyboard.h"

CKeyboard* CKeyboard::Instance = nullptr;

void CKeyboard::Create()
{
	assert(Instance == nullptr);

	Instance = new CKeyboard();
}

void CKeyboard::Destroy()
{
	assert(Instance != nullptr);

	delete Instance;
	Instance = nullptr;
}

CKeyboard* CKeyboard::Get()
{
	return Instance;
}

bool CKeyboard::Down(int InKey)
{
	if (GetAsyncKeyState(InKey) & 0x8000)
	{
		if (DownList[InKey] == false)
		{
			DownList.set(InKey, true);

			return true;
		}
	}
	else
	{
		DownList.set(InKey, false);
	}

	return false;
}

bool CKeyboard::Up(int InKey)
{
	if (GetAsyncKeyState(InKey) & 0x8000)
	{
		DownList.set(InKey, true);
	}
	else
	{
		if (DownList[InKey] == true)
		{
			DownList.set(InKey, false);

			return true;
		}
	}

	return false;
}

bool CKeyboard::Press(int InKey)
{
	if (GetAsyncKeyState(InKey) & 0x8000)
		return true;

	return false;
}

bool CKeyboard::Toggle(int InKey)
{
	if (GetAsyncKeyState(InKey) & 0x0001)
		return true;

	return false;
}

CKeyboard::CKeyboard()
{
	for (int i = 0; i < MAX_KEY_COUNT; i++)
	{
		UpList.set(i, false);
		DownList.set(i, false);
	}
}

CKeyboard::~CKeyboard()
{
}
