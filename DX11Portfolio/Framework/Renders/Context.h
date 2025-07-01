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
	float GetThreshold() { return SamplingData.threshold; }
	float GetStrength() { return SamplingData.strength; }

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

	// 사실 필터 GUI설정을 여기서 하는게 맞나 싶다.
	struct SamplingPixelConstantData
	{
		float dx;
		float dy;
		float threshold;
		float strength;
		Vector4 options;
	};
	SamplingPixelConstantData SamplingData;

private:
	std::unique_ptr<CCamera> View;
	SimpleMath::Matrix Projection;
	std::unique_ptr<D3D11_VIEWPORT> Viewport;

	ComPtr<ID3D11Buffer> m_constantBuffer;

	ViewProjectionConstantBuffer m_constantBufferData;
};