#include "Framework.h"
#include "Mouse.h"

CMouse* CMouse::Instance = nullptr;

void CMouse::Create()
{
	assert(Instance == nullptr);

	Instance = new CMouse();
}

void CMouse::Destroy()
{
	assert(Instance != nullptr);

	delete Instance;
	Instance = nullptr;
}

CMouse* CMouse::Get()
{
	return Instance;
}

bool CMouse::Down(int InType)
{
	return Down((EMouseButton)InType);
}

bool CMouse::Up(int InType)
{
	return Up((EMouseButton)InType);
}

bool CMouse::Press(int InType)
{
	return Press((EMouseButton)InType);
}

bool CMouse::Down(EMouseButton InType)
{
	return ButtonMaps[(int)InType] == EMouseButtonState::Down;
}

bool CMouse::Up(EMouseButton InType)
{
	return ButtonMaps[(int)InType] == EMouseButtonState::Up;
}

bool CMouse::Press(EMouseButton InType)
{
	return ButtonMaps[(int)InType] == EMouseButtonState::Press;
}

void CMouse::Tick()
{
	memcpy(ButtonOldStatus, ButtonStatus, sizeof(BYTE) * (int)EMouseButton::Max);

	ZeroMemory(ButtonStatus, sizeof(BYTE) * (int)EMouseButton::Max);
	ZeroMemory(ButtonMaps, sizeof(EMouseButtonState) * (int)EMouseButton::Max);

	ButtonStatus[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000 ? 1 : 0;
	ButtonStatus[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000 ? 1 : 0;
	ButtonStatus[2] = GetAsyncKeyState(VK_MBUTTON) & 0x8000 ? 1 : 0;

	for (int i = 0; i < (int)EMouseButton::Max; i++)
	{
		int status = ButtonStatus[i];
		int oldStatus = ButtonOldStatus[i];

		if (oldStatus == 0 && status == 1)
			ButtonMaps[i] = EMouseButtonState::Down;
		else if (oldStatus == 1 && status == 0)
			ButtonMaps[i] = EMouseButtonState::Up;
		else if (oldStatus == 1 && status == 1)
			ButtonMaps[i] = EMouseButtonState::Press;
		else
			ButtonMaps[i] = EMouseButtonState::None;
	}


	POINT point;
	GetCursorPos(&point);
	ScreenToClient(D3D::GetDesc().Handle, &point);

	WheelOldStatus.x = WheelStatus.x;
	WheelOldStatus.y = WheelStatus.y;

	WheelStatus.x = (float)point.x;
	WheelStatus.y = (float)point.y;

	WheelMoveDelta = WheelStatus - WheelOldStatus;
	WheelOldStatus.z = WheelStatus.z;
}

void CMouse::WndProc(UINT InMessage, WPARAM InwParam, LPARAM InlParam)
{
	if (InMessage == WM_MOUSEMOVE)
	{
		Position.x = (float)LOWORD(InlParam);
		Position.y = (float)HIWORD(InlParam);
	}

	if (InMessage == WM_MOUSEWHEEL)
	{
		WheelOldStatus.z = WheelStatus.z;
		WheelStatus.z += (float)((short)HIWORD(InwParam));
	}
}

CMouse::CMouse()
{
	Position = Vector3(0, 0, 0);

	ZeroMemory(ButtonStatus, sizeof(BYTE) * (int)EMouseButton::Max);
	ZeroMemory(ButtonOldStatus, sizeof(BYTE) * (int)EMouseButton::Max);
	ZeroMemory(ButtonMaps, sizeof(EMouseButtonState) * (int)EMouseButton::Max);


	WheelStatus = Vector3(0, 0, 0);
	WheelOldStatus = Vector3(0, 0, 0);
	WheelMoveDelta = Vector3(0, 0, 0);
}

CMouse::~CMouse()
{

}
