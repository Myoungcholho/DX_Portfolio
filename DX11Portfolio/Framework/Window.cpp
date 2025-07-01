#include "Framework.h"
#include "Window.h"
#include "IExecutable.h"

WPARAM Window::Run(IExecutable* InMain)
{
	return WPARAM();
}

void Window::Create()
{
}

void Window::Destroy()
{
}

LRESULT Window::WndProc(HWND InHandle, UINT InMessage, WPARAM InwParam, LPARAM InlParam)
{
	return LRESULT();
}

void Window::MainRender()
{
}
