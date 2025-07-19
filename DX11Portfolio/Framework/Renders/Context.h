#pragma once
#include <directxtk/SimpleMath.h>
#include <memory>

struct ViewProjectionConstantBuffer
{
	Matrix view;
	Matrix projection;
};

class CContext
{
public:
	static void Create();
	static void Destroy();

	static CContext* Get();

private:
	CContext();
	~CContext();

public:
	void Tick();
	void Tick_View();
	void Tick_WorldTime();

	void Render();

	void ResizeScreen();

public:
	class CCamera* GetCamera() { return View.get(); }
	bool GetWireRender() { return bWireRender; }

public:
	SimpleMath::Matrix GetViewMatrix();
	SimpleMath::Matrix GetProjectionMatrix();

public:
	void SetHideSetting(bool InValue) { bHideSetting = InValue; }

private:
	static CContext* Instance;

private:
	bool bHideSetting = false;
	bool bWireRender = false;

private:
	std::unique_ptr<CCamera> View;
	SimpleMath::Matrix Projection;
	std::unique_ptr<D3D11_VIEWPORT> Viewport;

	ComPtr<ID3D11Buffer> m_constantBuffer;

	ViewProjectionConstantBuffer m_constantBufferData;
};