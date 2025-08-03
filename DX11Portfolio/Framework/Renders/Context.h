#pragma once
#include <directxtk/SimpleMath.h>
#include <memory>

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
	void RenderConstantBuffer();
	void RenderMirrorConstantBuffer();

	void ResizeScreen();

public:
	void UpdateMirror(const Matrix& mirror);

public:
	class CCamera* GetCamera() { return View.get(); }

public:
	SimpleMath::Matrix GetViewMatrix();
	SimpleMath::Matrix GetProjectionMatrix();
	//SimpleMath::Matrix GetMirrorMatrix();

public:
	void SetHideSetting(bool InValue) { bHideSetting = InValue; }

private:
	static CContext* Instance;

private:
	bool bHideSetting = false;

private:
	std::unique_ptr<CCamera> View;
	SimpleMath::Matrix Projection;
	std::unique_ptr<D3D11_VIEWPORT> Viewport;

private:
	//Matrix mirror;

private:
	//ViewProjectionConstantBuffer m_constantBufferData;
	//MirrorViewProjectionConstantBuffer mirrorViewProjectionConstantBufferData;

	//ComPtr<ID3D11Buffer> m_constantBuffer;
	//ComPtr<ID3D11Buffer> mirrorViewProjectionConstantBuffer;
};