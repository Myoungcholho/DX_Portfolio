#include "Framework.h"
#include "Window.h"
#include "IExecutable.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
	LPARAM lParam);

IExecutable* Window::Executor = nullptr;
HWND Window::Handle = nullptr;
WindowDesc Window::Desc = {};

// 실제 게임 Loop
WPARAM Window::Run(IExecutable* InExecutor)
{
	CKeyboard::Create();
	CTimer::Create();
	CMouse::Create();
	CContext::Create();
	Engine::Create();

	D3D::Get()->Init();

	Executor = InExecutor;
	Executor->Initialize();

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (true) //Game Loop
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg); //WM_CHAR
			DispatchMessage(&msg);
		}
		else
		{
			MainRender();
		}
	}

	Executor->Destroy();

	Engine::Destory();
	CContext::Destroy();
	CMouse::Destroy();
	CTimer::Destroy();
	CKeyboard::Destroy();

	return msg.wParam;
}

HWND Window::Create(const WindowDesc& inDesc)
{
	WNDCLASSEX wc = 
	{
		sizeof(WNDCLASSEX), 
		CS_HREDRAW | CS_VREDRAW, 
		WndProc,
		0, 
		0, 
		inDesc.Instance, 
		LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW),
	   (HBRUSH)(COLOR_WINDOW + 1), 
	   nullptr, 
	   inDesc.AppName.c_str(),
	   LoadIcon(nullptr, IDI_APPLICATION) 
	};

	RegisterClassEx(&wc);

	HWND hwnd = CreateWindow(inDesc.AppName.c_str(), inDesc.AppName.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		(int)inDesc.Width, (int)inDesc.Height, nullptr, nullptr, inDesc.Instance, nullptr);

	RECT rect = { 0, 0, (LONG)inDesc.Width, (LONG)inDesc.Height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	MoveWindow(hwnd, (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2,
		rect.right - rect.left, rect.bottom - rect.top, TRUE);

	ShowWindow(hwnd, SW_SHOWNORMAL);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	Handle = hwnd;
	return hwnd;
}

void Window::Destroy()
{
	DestroyWindow(Handle);
	UnregisterClass(Desc.AppName.c_str(), Desc.Instance);
}

LRESULT Window::WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam)
{
	CMouse::Get()->WndProc(InMessage, InwParam, InlParam);
	
	if (ImGui_ImplWin32_WndProcHandler(InHandle, InMessage, InwParam, InlParam))
		return true;

	if (InMessage == WM_SIZE)
	{
		if (Executor != nullptr)
		{
			float width = (float)LOWORD(InlParam);
			float height = (float)HIWORD(InlParam);

			if (D3D::Get() != nullptr)
				D3D::Get()->ResizeScreen(width, height);

			if (CContext::Get() != nullptr)
				CContext::Get()->ResizeScreen();
		}
	}

	if (InMessage == WM_KEYDOWN)
	{
		if (InwParam == VK_ESCAPE)
		{
			PostQuitMessage(0);

			return 0;
		}
	}

	if (InMessage == WM_CLOSE || InMessage == WM_DESTROY)
	{
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(InHandle, InMessage, InwParam, InlParam);
}

void Window::MainRender()
{
	ImGuiManager::BeginFrame();

	D3D::Get()->UpdateGUI();
	Executor->UpdateGUI();

	CTimer::Get()->Tick();
	CMouse::Get()->Tick();
	CContext::Get()->Tick();
	Executor->Tick();


	//Rendering
	{
		D3D::Get()->StartRenderPass();

		if (CContext::Get()->GetWireRender() == true)
			GraphicsDevice::Get()->ApplyWireframeRasterizer();
		else
			GraphicsDevice::Get()->ApplySolidRasterizer();


		CContext::Get()->Render();
		
		Executor->Render();

		D3D::Get()->Render();
	}

	ImGuiManager::Get()->Render();
	ImGuiManager::EndFrame();
	D3D::Get()->Present();
}
