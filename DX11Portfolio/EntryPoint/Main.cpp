#include "Pch.h"
#include "Main.h"
#include "Systems/Window.h"
#include <memory>
#include "ExecutionManager.h"

int WINAPI WinMain(HINSTANCE InInstance, HINSTANCE InPrevInstance, LPSTR InParam, int command)
{
	EnableConsole();

	WindowDesc winDesc;
	winDesc.AppName = L"DirectXPortfolio";
	winDesc.Instance = InInstance;
	winDesc.Width = 1920; // 1280 -> 1920
	winDesc.Height = 1080; // 720 -> 1080
	winDesc.Background = Color(0.2f, 0.2f, 0.2f, 1.0f);

	// 윈도우 생성
	HWND hwnd = Window::Create(winDesc);

	// D3D 초기화
	D3DDesc d3dDesc = { hwnd, winDesc.Width, winDesc.Height, true };
	D3D::SetDesc(d3dDesc);
	D3D::Create();

	// 전역 GraphicsDevice 초기화
	GraphicsDevice::Create();

	// ImGUI 초기화
	ImGUIDesc imguiDesc = {};
	imguiDesc.Device = D3D::Get()->GetDeviceCom();
	imguiDesc.DeviceContext = D3D::Get()->GetDeviceContextCom();
	imguiDesc.Width = winDesc.Width;
	imguiDesc.Height = winDesc.Height;
	imguiDesc.Hwnd = hwnd;
	ImGuiManager::SetDesc(imguiDesc);
	ImGuiManager::Create();


	// Game Run
	auto executor = std::make_unique<ExecutionManager>();
	WPARAM result = Window::Run(executor.get());

	ImGuiManager::Destroy();
	GraphicsDevice::Destory();
	D3D::Destroy();
	Window::Destroy();

	return (int)result;
}