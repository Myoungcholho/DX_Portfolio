#include "Framework.h"
#include "D3D.h"
#include "DirectXTK/DDSTextureLoader.h"

D3D* D3D::Instance = nullptr;
D3DDesc D3D::D3dDesc = D3DDesc();

D3D* D3D::Get()
{
	//assert(Instance != nullptr);
	return Instance;
}

void D3D::Create()
{
	assert(Instance == nullptr);
	Instance = new D3D();
}

void D3D::Destroy()
{
	if (Instance != nullptr)
	{
		delete Instance;
		Instance = nullptr;
	}
}

const D3DDesc& D3D::GetDesc()
{
	return D3dDesc;
}

void D3D::SetDesc(const D3DDesc& InDesc)
{
	D3dDesc = InDesc;
}

void D3D::SaveAndBindTempDepthStencil()
{
	//// 1. 가지고 와서 temp에 DSV를 복사한다
	//DeviceContext->CopyResource(Temp_DSV_Texture.Get(), DSV_Texture.Get());

	//// 2. 원본 DSV정보는 유지할수있도록하기 위해 복사한 DSV를 사용한다
	//const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//vector<ID3D11RenderTargetView*> renderTargetViews = { m_floatRTV.Get() };
	//DeviceContext->OMSetRenderTargets(UINT(renderTargetViews.size()), renderTargetViews.data(), Temp_DepthStencilView.Get());
}

void D3D::RestoreOriginalDepthStencil()
{
	//// 1. 원래 쓰던 DSV를 OM에 바인딩
	//const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//vector<ID3D11RenderTargetView*> renderTargetViews = { m_floatRTV.Get() };
	//DeviceContext->OMSetRenderTargets(UINT(renderTargetViews.size()), renderTargetViews.data(), DepthStencilView.Get());
}

void D3D::CreateDevice()
{
	DXGI_MODE_DESC desc = {};
	desc.Width = (UINT)D3dDesc.Width;
	desc.Height = (UINT)D3dDesc.Height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.RefreshRate.Numerator = 0;			// 0은 자동으로 현재 모니터 주사율 사용
	desc.RefreshRate.Denominator = 1;		// 주사율 분모 현재 0(자동->75)/1 = 75hz

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc = desc;
	swapChainDesc.BufferCount = 2;			// 백버퍼 사용 개수
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = D3dDesc.Handle;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;


	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		SwapChain.GetAddressOf(),
		Device.GetAddressOf(),
		nullptr,
		DeviceContext.GetAddressOf()
	);
	assert(SUCCEEDED(hr) && "Device creation failed");
}

void D3D::CreateBuffers()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	// 1. 스왑체인으로부터 백버퍼를 얻고 텍스처에 저장
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	// 2. 백버퍼로 RTV를 만듬
	Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_backBufferRTV.GetAddressOf());
	// 3. 내 GPU 드라이버가 제공하는 품질 수준 가능 범위 반환 및 float텍스처 생성
	Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &m_numQualityLevels);
	
	/*D3D11_TEXTURE2D_DESC desc;
	backBuffer->GetDesc(&desc);
	desc.MipLevels = desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;
	if (m_useMSAA && m_numQualityLevels)
	{
		desc.SampleDesc.Count = 4;
		desc.SampleDesc.Quality = m_numQualityLevels - 1;
	}
	else
	{
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
	}*/
	//ThrowIfFailed(Device->CreateTexture2D(&desc, NULL, m_floatBuffer.GetAddressOf()));
	//// 4. float Texture로 SRV 생성
	//Device->CreateShaderResourceView(m_floatBuffer.Get(), NULL, m_floatSRV.GetAddressOf());
	//// 5. float Texture로 RTV 생성
	//Device->CreateRenderTargetView(m_floatBuffer.Get(), NULL, m_floatRTV.GetAddressOf());
	// 6. DepthBuffer 생성
	CreateDepthBuffer();
	
	// 7. PostProcess를 위한 MSAA 제거 SRV/RTV
	/*desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	ThrowIfFailed(Device->CreateTexture2D(&desc, NULL, m_resolvedBuffer.GetAddressOf()));
	ThrowIfFailed(Device->CreateShaderResourceView(m_resolvedBuffer.Get(), NULL, m_resolvedSRV.GetAddressOf()));
	ThrowIfFailed(Device->CreateRenderTargetView(m_resolvedBuffer.Get(), NULL, m_resolvedRTV.GetAddressOf()));*/
}

void D3D::CreateDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	depthStencilBufferDesc.Width = (UINT)D3dDesc.Width;
	depthStencilBufferDesc.Height = (UINT)D3dDesc.Height;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (m_numQualityLevels > 0)
	{
		depthStencilBufferDesc.SampleDesc.Count = 4;
		depthStencilBufferDesc.SampleDesc.Quality = m_numQualityLevels - 1;
	}
	else
	{
		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;
	}
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	Device->CreateTexture2D(&depthStencilBufferDesc, 0, DSV_Texture.GetAddressOf());
	Device->CreateDepthStencilView(DSV_Texture.Get(), 0, DepthStencilView.GetAddressOf());

	/*Device->CreateTexture2D(&depthStencilBufferDesc, 0, Temp_DSV_Texture.GetAddressOf());
	Device->CreateDepthStencilView(Temp_DSV_Texture.Get(), 0, Temp_DepthStencilView.GetAddressOf());*/
}

ComPtr<ID3D11RenderTargetView> D3D::GetBackBufferRTV()
{
	return m_backBufferRTV;
}

ComPtr<ID3D11ShaderResourceView> D3D::GetBackBufferSRV()
{
	return m_backBufferSRV;
}

ComPtr<ID3D11DepthStencilView> D3D::GetDepthStencilView()
{
	return DepthStencilView;
}

void D3D::Present()
{
	HRESULT hr =  SwapChain->Present(1, 0);
	if (FAILED(hr))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_HUNG)
		{
			HRESULT reason = Device->GetDeviceRemovedReason();
			switch (reason) 
			{
			case DXGI_ERROR_DEVICE_HUNG: OutputDebugStringA("Device HUNG\n"); break;
			case DXGI_ERROR_DEVICE_REMOVED: OutputDebugStringA("Device REMOVED\n"); break;
			case DXGI_ERROR_DEVICE_RESET: OutputDebugStringA("Device RESET\n"); break;
			default: OutputDebugStringA("Unknown GPU error\n"); break;
			}
		}
	}
}

void D3D::ResizeScreen(float InWidth, float InHeight)
{
	if (InWidth < 1 || InHeight < 1)
		return;

	D3dDesc.Width = InWidth;
	D3dDesc.Height = InHeight;

	m_backBufferRTV.Reset();
	DSV_Texture.Reset();
	DepthStencilView.Reset();

	SwapChain->ResizeBuffers(0, (UINT)InWidth, (UINT)InHeight, DXGI_FORMAT_UNKNOWN, 0);

	CreateBuffers();
	
	if (OnReSizeDelegate.IsBound())
		OnReSizeDelegate.Broadcast();
}

ID3D11Texture2D* D3D::GetRTVTexture()
{
	ComPtr<ID3D11Texture2D> texture;
	m_backBufferRTV->GetResource(reinterpret_cast<ID3D11Resource**>(texture.GetAddressOf()));
	return texture.Detach();
}

D3D::D3D()
{
	CreateDevice();				// SwapChain, Device, Context 생성
	CreateBuffers();			// RTV들 생성
}

D3D::~D3D()
{
	DepthStencilView.Reset();
	DSV_Texture.Reset();
	m_backBufferRTV.Reset();
	DeviceContext.Reset();
	Device.Reset();
	SwapChain.Reset();
	//RasterizerState.Reset();
}

void D3D::GraphicsInit()
{
	Graphics::InitCommonStates(Device);
}