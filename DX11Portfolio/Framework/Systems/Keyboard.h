#pragma once

class CKeyboard
{
public:
	static void Create();
	static void Destroy();

	static CKeyboard* Get();

private:
	static CKeyboard* Instance;

public:
	bool Down(int InKey);
	bool Up(int InKey);

	bool Press(int InKey);
	bool Toggle(int InKey);

private:
	CKeyboard();
	~CKeyboard();

private:
	bitset<MAX_KEY_COUNT> UpList;
	bitset<MAX_KEY_COUNT> DownList;
};