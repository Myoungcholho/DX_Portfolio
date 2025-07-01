#pragma once

struct WindowDesc 
{
	std::wstring AppName;
	HINSTANCE Instance;
	float Width;
	float Height;
	Color Background;
};

class Window
{
public:
	static WPARAM Run(class IExecutable* InMain);

	static HWND Create(const WindowDesc& inDesc);
	static void Destroy();
private:

	static LRESULT CALLBACK WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam);

	static void MainRender();

private:
	static class IExecutable* Executor;
	static HWND Handle;
	static WindowDesc Desc;
};