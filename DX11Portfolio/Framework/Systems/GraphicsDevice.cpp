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

///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
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

void GraphicsDevice::ApplySolidCCWRasterizer()
{
	D3D::Get()->GetDeviceContext()->RSSetState(SolidCCWRS.Get());
}

void GraphicsDevice::ApplyWireframeCCWRasterizer()
{
	D3D::Get()->GetDeviceContext()->RSSetState(WireFrameCCWRS.Get());
}
///////////////////////////////////////////////////////////////////////////////

void GraphicsDevice::ApplyDrawBasicDSS(int stencilRef)
{
	D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(DrawBasicDSS.Get(), stencilRef);
}
void GraphicsDevice::ApplyMaskDSS(int stencilRef)
{
	D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(MaskDSS.Get(), stencilRef);
}
void GraphicsDevice::ApplyDrawMaskedDSS(int stencilRef)
{
	D3D::Get()->GetDeviceContext()->OMSetDepthStencilState(DrawMaskedDSS.Get(), stencilRef);
}

///////////////////////////////////////////////////////////////////////////////
void GraphicsDevice::ApplyBasicBlendState()
{
	D3D::Get()->GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xffffffff);
}
void GraphicsDevice::ApplyMirrorBlendState(const float* blendColor)
{
	D3D::Get()->GetDeviceContext()->OMSetBlendState(MirrorBlendState.Get(), blendColor, 0xffffffff);
}

///////////////////////////////////////////////////////////////////////////////
GraphicsDevice::GraphicsDevice()
{
	// RS
	CreateBasicRasterizerState();
	CreateWireRasterizerState();
	CreateNoCullNoClipRasterizerState();
	CreateBasicCCWRasterizerState();
	CreateWireCCWRasterizerState();

	// Sampler
	CreateLinearWrapSamplerState();
	CreateLinearWrapMipPointSamplerState();
	CreateLinearClampSamplerState();

	// DepthStencil
	CreateDrawBasicDepthStencilState();
	CreateMaskDepthStencilState();
	CreateDrawMaksedDepthStencilState();

	// Blend
	CreateMirrorAlphaBlendState();
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
	desc.MultisampleEnable = true;

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
	desc.MultisampleEnable = true;

	HRESULT hr = D3D::Get()->GetDevice()->CreateRasterizerState(&desc, WireFrameRS.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void GraphicsDevice::CreateBasicCCWRasterizerState()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = true;
	desc.DepthClipEnable = true;			// MinDepth ~ MaxDepth 밖이라면 그리지 않겠다!
	desc.MultisampleEnable = true;

	HRESULT hr = D3D::Get()->GetDevice()->CreateRasterizerState(&desc, SolidCCWRS.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void GraphicsDevice::CreateWireCCWRasterizerState()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = true;
	desc.DepthClipEnable = true;
	desc.MultisampleEnable = true;

	HRESULT hr = D3D::Get()->GetDevice()->CreateRasterizerState(&desc, WireFrameCCWRS.GetAddressOf());
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

///////////////////////////////////////////////////////////////////////////////

void GraphicsDevice::CreateDrawBasicDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = true;										// depth 사용O
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;				// 모든 픽셀에 깊이값 기록
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;						// 새 픽셀의 깊이가 현재 값보다 작을 때만 통과
	dsDesc.StencilEnable = false;									// 스텐실 사용X
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;		// 기본값사용
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;		// 기본값사용
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;			// [앞면]스텐실 테스트 실패 시 현재 값 유지
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	// [앞면]스텐실 통과 & 깊이 테스트 실패 시 현재 값 유지
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;			// [앞면]둘 다 통과 시 현재 값 유지
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;			// [앞면]항상 통과
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;			// [뒷면]스텐실 테스트 실패 시 현재 값 유지
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		// [뒷면]스텐실 통과 & 깊이 테스트 실패 시 현재 값 유지
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		// [뒷면]둘 다 통과 시 스텐실 값을 새 값으로 교체
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;			// [뒷면]항상 통과(조건 없이 연산)
	
	D3D::Get()->GetDevice()->CreateDepthStencilState(&dsDesc, DrawBasicDSS.GetAddressOf());
}

void GraphicsDevice::CreateMaskDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = true;										// depth 사용O
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;			// depth 비교는 수행하나 Write는 하지 않음
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;						// 새 픽셀의 깊이가 현재 값보다 작을 때만 통과
	dsDesc.StencilEnable = true;									// 스텐실 사용함
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;		// 스텐실 비교 ex)MASK 0x1 사용하면 하위 1비트만 비교함 
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_READ_MASK;		// 스텐실 비교 ex)MASK 0x1 사용하면 하위 1비트만 비교함
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;			// [앞면]스텐실 테스트 실패 시 현재 값 유지
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	// [앞면]스텐실 통과 & 깊이 테스트 실패 시 현재 값 유지
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		// [앞면]조건이 맞을 때마다 예전 값과 상관없이 새 참조값으로 계속 덮어쓰기
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;			// [앞면]스텐실 비교 기준(항상 통과)
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;			// [뒷면]스텐실 테스트 실패 시 현재 값 유지
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		// [뒷면]스텐실 통과 & 깊이 테스트 실패 시 현재 값 유지
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		// [뒷면]둘 다 통과 시 스텐실 값을 새 값으로 교체
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;			// [뒷면]

	
	D3D::Get()->GetDevice()->CreateDepthStencilState(&dsDesc, MaskDSS.GetAddressOf());
}

void GraphicsDevice::CreateDrawMaksedDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = true;										// depth 사용O
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;				//
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;					// 새 픽셀이 스텐실 값과 같을때만 통과
	dsDesc.StencilEnable = true;									// 스텐실 사용함
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;		// 스텐실 비교 ex)MASK 0x1 사용하면 하위 1비트만 비교함 
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_READ_MASK;		// 스텐실 비교 ex)MASK 0x1 사용하면 하위 1비트만 비교함
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;			// [앞면]스텐실 테스트 실패 시 현재 값 유지
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	// [앞면]스텐실 통과 & 깊이 테스트 실패 시 현재 값 유지
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;			// 
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;			// 
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;			// [뒷면]스텐실 테스트 실패 시 현재 값 유지
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;		// [뒷면]스텐실 통과 & 깊이 테스트 실패 시 현재 값 유지
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;		// [뒷면]둘 다 통과 시 스텐실 값을 새 값으로 교체
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;			// [뒷면]

	D3D::Get()->GetDevice()->CreateDepthStencilState(&dsDesc, DrawMaskedDSS.GetAddressOf());
}

///////////////////////////////////////////////////////////////////////////////

void GraphicsDevice::CreateMirrorAlphaBlendState()
{
	D3D11_BLEND_DESC mirrorBlendDesc;
	ZeroMemory(&mirrorBlendDesc, sizeof(mirrorBlendDesc));

	mirrorBlendDesc.AlphaToCoverageEnable = true;											// MSAA에서 투명 경계 부드럽게 처리
	mirrorBlendDesc.IndependentBlendEnable = false;											// 모든 RenderTaget에 동일한 블렌드 규칙 사용
	mirrorBlendDesc.RenderTarget[0].BlendEnable = true;										// 블렌드 활성화
	mirrorBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_BLEND_FACTOR;				// 1 - blendcolor로 섞음, 0.2라면 0.8비율로 투명오브젝트가 섞임, alpha로 섞을려면 SRC_ALPHA를 써야함
	mirrorBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR;					// alpha로 섞음, Inv쓸순있지만 값이 이상해짐
	mirrorBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;							// 최종 색상 = (소스 * srcBlend) + (배경 * destBlend) 여기서 연산 기준

	mirrorBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;						// 거울효과는 누적 (1-alpha)를 안쓰고 둘다 *1을함
	mirrorBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;						// 거울효과는 누적
	mirrorBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;						

	mirrorBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// RTV에 데이터를 쓸때 RGBA 모두 작성

	D3D::Get()->GetDevice()->CreateBlendState(&mirrorBlendDesc, MirrorBlendState.GetAddressOf());
}