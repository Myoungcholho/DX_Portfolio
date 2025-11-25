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
	UINT numQualityLevels = 0;
	bool useMSAA = true;

private:
	D3D();
	~D3D();

public:
	void GraphicsInit();

private:
	static D3D* Instance;

private:
	static D3DDesc D3dDesc;

public:
	ID3D11Device* GetDevice() { return device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return deviceContext.Get(); }
	ComPtr<ID3D11Device> GetDeviceCom() const { return device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContextCom() const { return deviceContext; }

	ComPtr<ID3D11RenderTargetView> GetBackBufferRTV(){ return backBufferRTV; }
	ComPtr<ID3D11DepthStencilView> GetDepthStencilView(){ return depthStencilView; }

	ComPtr<ID3D11RenderTargetView> GetFinalLDR_RTV() { return finalLDR_RTV;}
	ComPtr<ID3D11ShaderResourceView> GetFinalLDR_SRV() { return finalLDR_SRV; }

	ComPtr<ID3D11DepthStencilView> GetDepthOnly_DSV() { return depthOnlyDSV; }
	ComPtr<ID3D11ShaderResourceView> GetDepthOnly_SRV() { return depthOnlySRV; }

private:
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<ID3D11RenderTargetView> backBufferRTV;
	ComPtr<ID3D11Texture2D> DSV_Texture;
	ComPtr<ID3D11DepthStencilView> depthStencilView;

	// Depth Buffer 관련
	ComPtr<ID3D11Texture2D> depthOnlyBuffer;
	ComPtr<ID3D11DepthStencilView> depthOnlyDSV;
	ComPtr<ID3D11ShaderResourceView> depthOnlySRV;

	// SceneView를 위해
	ComPtr<ID3D11Texture2D> finalLDR;
	ComPtr<ID3D11RenderTargetView> finalLDR_RTV;
	ComPtr<ID3D11ShaderResourceView> finalLDR_SRV;

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;
};