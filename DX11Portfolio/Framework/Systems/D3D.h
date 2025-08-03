#pragma once

#include <iostream>
#include <iomanip>

struct D3DDesc 
{
	HWND Handle; // SwapChain, Resize, Present에서 윈도우 핸들 사용하기 때문

	float Width;
	float Height;
	bool VSync;
};

/// <summary>
/// Low-Level Device/Context 및 기본 리소스의 생성/소유/제공자 역할만을 수행
/// </summary>
class D3D
{
public:
	static D3D* Get();

public:
	static void Create();
	static void Destroy();

public:
	static const D3DDesc& GetDesc();
	static void SetDesc(const D3DDesc& InDesc);

public:
	void SaveAndBindTempDepthStencil();
	void RestoreOriginalDepthStencil();

private:
	void CreateDevice();
	void CreateBuffers();
	void CreateDepthBuffer();
	
public:
	//void StartRenderPass();
	//void ClearMainDepth(D3D11_CLEAR_FLAG ClearFlags, FLOAT Depth, UINT8 Stencil);
	//void ClearTempDepth(D3D11_CLEAR_FLAG ClearFlags, FLOAT Depth, UINT8 Stencil);

public:
	void Present();

	void ResizeScreen(float InWidth, float InHeight);

public:
	ID3D11Texture2D* GetRTVTexture();	

public:
	FDynamicMulticastDelegate OnReSizeDelegate;

public:
	UINT m_numQualityLevels = 0;
	bool m_useMSAA = true;

private:
	D3D();
	~D3D();

public:
	void GraphicsInit();
	//void UpdateGUI();
	//void Render();

private:
	static D3D* Instance;

private:
	static D3DDesc D3dDesc;

public:
	ID3D11Device* GetDevice() { return Device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return DeviceContext.Get(); }
	ComPtr<ID3D11Device> GetDeviceCom() const { return Device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContextCom() const { return DeviceContext; }

	ComPtr<ID3D11RenderTargetView> GetBackBufferRTV();
	ComPtr<ID3D11ShaderResourceView> GetBackBufferSRV();
	ComPtr<ID3D11DepthStencilView> GetDepthStencilView();


private:
	ComPtr<IDXGISwapChain> SwapChain;
	ComPtr<ID3D11RenderTargetView> m_backBufferRTV;
	ComPtr<ID3D11ShaderResourceView> m_backBufferSRV;
	ComPtr<ID3D11Texture2D> DSV_Texture;
	ComPtr<ID3D11DepthStencilView> DepthStencilView;

	ComPtr<ID3D11Device> Device;
	ComPtr<ID3D11DeviceContext> DeviceContext;
};