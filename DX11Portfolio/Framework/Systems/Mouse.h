#pragma once

#include <directxtk/SimpleMath.h>
using namespace DirectX::SimpleMath;

enum class EMouseButton
{
	Left = 0, Right, Middle, Max,
};

class CMouse
{
public:
	static void Create();
	static void Destroy();

	static CMouse* Get();

public:
	Vector3 GetPosition() { return Position; }
	Vector3 GetMoveDelta() { return WheelMoveDelta; }

public:
	bool Down(int InType);
	bool Up(int InType);
	bool Press(int InType);

	bool Down(EMouseButton InType);
	bool Up(EMouseButton InType);
	bool Press(EMouseButton InType);

public:
	void Tick();

	void WndProc(UINT InMessage, WPARAM InwParam, LPARAM InlParam);

private:
	static CMouse* Instance;

private:
	CMouse();
	~CMouse();

private:
	enum class EMouseButtonState
	{
		None = 0, Down, Up, Press, DoubleClick, Max,
	};

private:
	Vector3 Position;

	BYTE ButtonStatus[(int)EMouseButton::Max];
	BYTE ButtonOldStatus[(int)EMouseButton::Max];
	EMouseButtonState ButtonMaps[(int)EMouseButton::Max];

	Vector3 WheelStatus;
	Vector3 WheelOldStatus;
	Vector3 WheelMoveDelta;
};
