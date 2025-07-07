#include "Framework.h"
#include "GraphicsDevice.h"

GraphicsDevice* GraphicsDevice::Instance = nullptr;

GraphicsDevice* GraphicsDevice::Get()
{
	assert(Instance != nullptr);
	return Instance;
}

void GraphicsDevice::Create()
{
	assert(Instance == nullptr);
	Instance = new GraphicsDevice();
}

void GraphicsDevice::Destory()
{
	if (Instance != nullptr)
	{
		delete Instance;
		Instance = nullptr;
	}
}

ID3D11SamplerState*const* GraphicsDevice::GetLinearWrapSampler() const
{
	return LinearWrap.GetAddressOf();
}

ID3D11SamplerState* const* GraphicsDevice::GetLinearMipPointWrapSampler() const
{
	return LinearMipPointWrap.GetAddressOf();
}

ID3D11SamplerState* const* GraphicsDevice::GetLinearClampSampler() const
{
	return LinearClamp.GetAddressOf();
}

void GraphicsDevice::ApplySolidRasterizer()
{
	D3D::Get()->GetDeviceContext()->RSSetState(SolidRS.Get());
}

void GraphicsDevice::ApplyWireframeRasterizer()
{
	D3D::Get()->GetDeviceContext()->RSSetState(WireFrameRS.Get());
}

void GraphicsDevice::ApplyNoCullNoClipRasterizer()
{
	D3D::Get()->GetDeviceContext()->RSSetState(NoCullNoClipRS.Get());
}

GraphicsDevice::GraphicsDevice()
{
	CreateBasicRasterizerState();
	CreateWireRasterizerState();
	CreateNoCullNoClipRasterizerState();

	CreateLinearWrapSamplerState();
	CreateLinearWrapMipPointSamplerState();
	CreateLinearClampSamplerState();
}

GraphicsDevice::~GraphicsDevice()
{

}

///////////////////////////////////////////////////////////////////////////////

void GraphicsDevice::CreateBasicRasterizerState()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;
	desc.DepthClipEnable = true;			// MinDepth ~ MaxDepth 밖이라면 그리지 않겠다!

	HRESULT hr = D3D::Get()->GetDevice()->CreateRasterizerState(&desc, SolidRS.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void GraphicsDevice::CreateWireRasterizerState()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = false;
	desc.DepthClipEnable = true;

	HRESULT hr = D3D::Get()->GetDevice()->CreateRasterizerState(&desc, WireFrameRS.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void GraphicsDevice::CreateNoCullNoClipRasterizerState()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = false;
	desc.DepthClipEnable = false;

	HRESULT hr = D3D::Get()->GetDevice()->CreateRasterizerState(&desc, NoCullNoClipRS.GetAddressOf());
	assert(SUCCEEDED(hr));
}

///////////////////////////////////////////////////////////////////////////////

void GraphicsDevice::CreateLinearWrapSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	D3D::Get()->GetDevice()->CreateSamplerState(&sampDesc, LinearWrap.GetAddressOf());
}

void GraphicsDevice::CreateLinearWrapMipPointSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	D3D::Get()->GetDevice()->CreateSamplerState(&sampDesc, LinearMipPointWrap.GetAddressOf());
}

void GraphicsDevice::CreateLinearClampSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	D3D::Get()->GetDevice()->CreateSamplerState(&sampDesc, LinearClamp.GetAddressOf());
}