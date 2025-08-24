#include "Framework.h"
#include "Window.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,LPARAM lParam);

HWND Window::Handle = nullptr;
WindowDesc Window::Desc = {};

// 실제 게임 Loop
WPARAM Window::Run(UGameInstance* game)
{
	EditorApplication::SetWorld(game->GetWorld());
	EditorApplication::SetRenderer(game->GetRenderer());
	EditorApplication::Initialize();

	CKeyboard::Create();
	CTimer::Create();
	CMouse::Create();
	CContext::Create();
	Engine::Create();

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
			MainRender(game);
		}
	}

	game->Destroy();

	Engine::Destory();
	CContext::Destroy();
	CMouse::Destroy();
	CTimer::Destroy();
	CKeyboard::Destroy();

	EditorApplication::Release();
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

LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))	// ImGui에 들어온 입력이라면 아래 로직은 처리하지 않겠다
		return 0;

	CMouse::Get()->WndProc(message, wParam, lParam);
	
	switch (message)
	{
	case WM_SIZE:
	{
		float width = static_cast<float>(LOWORD(lParam));
		float height = static_cast<float>(HIWORD(lParam));
		
		// 내부에서 0 Guard도 하고
		// 브로드 캐스트도 하고 있음
		if (D3D::Get() != nullptr)
			D3D::Get()->ResizeScreen(width, height);

		break;
	}

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			return 0;
		}
		break;

	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, NULL,
				suggested_rect->left, suggested_rect->top,
				suggested_rect->right - suggested_rect->left,
				suggested_rect->bottom - suggested_rect->top,
				SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;

	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Window::MainRender(UGameInstance* game)
{
	//ImGuiManager::Get()->BeginFrame(); // 싱글톤 Tick에서 호출됨

	//game->OnGUI();

	CTimer::Get()->Tick();
	CMouse::Get()->Tick();
	CContext::Get()->Tick();

	game->Tick();

	//CContext::Get()->Render();
	//ImGuiManager::Get()->EndFrame();

	game->Render();
}
