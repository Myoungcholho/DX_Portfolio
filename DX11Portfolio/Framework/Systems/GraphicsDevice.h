#pragma once

class GraphicsDevice
{
public:
	static GraphicsDevice* Get();

public:
	static void Create();
	static void Destory();

public:
	// SamplerState Get
	ID3D11SamplerState* const* GetLinearWrapSampler() const;
	ID3D11SamplerState* const* GetLinearMipPointWrapSampler() const;
	ID3D11SamplerState* const* GetLinearClampSampler() const;

public:
	// RSState 적용
	void ApplySolidRasterizer();
	void ApplyWireframeRasterizer();
	void ApplyNoCullNoClipRasterizer();
	void ApplySolidCCWRasterizer();
	void ApplyWireframeCCWRasterizer();
public:
	// DepthStencil 적용
	void ApplyDrawBasicDSS(int stencilRef);
	void ApplyMaskDSS(int stencilRef);
	void ApplyDrawMaskedDSS(int stencilRef);

	// Blend 적용
	void ApplyBasicBlendState();
	void ApplyMirrorBlendState(const float* blendColo);

private:
	GraphicsDevice();
	~GraphicsDevice();

private:
	void CreateBasicRasterizerState();
	void CreateWireRasterizerState();
	void CreateBasicCCWRasterizerState();
	void CreateWireCCWRasterizerState();

	void CreateNoCullNoClipRasterizerState();

	// Sampler
	void CreateLinearWrapSamplerState();
	void CreateLinearWrapMipPointSamplerState();
	void CreateLinearClampSamplerState();

	// DepthStencilState
	void CreateDrawBasicDepthStencilState();
	void CreateMaskDepthStencilState();
	void CreateDrawMaksedDepthStencilState();

	// BlendState
	void CreateMirrorAlphaBlendState();

private:
	static GraphicsDevice* Instance;

private:
	ComPtr<ID3D11RasterizerState> SolidRS;				// 시계
	ComPtr<ID3D11RasterizerState> WireFrameRS;			// 시계
	ComPtr<ID3D11RasterizerState> SolidCCWRS;			// 반시계
	ComPtr<ID3D11RasterizerState> WireFrameCCWRS;		// 반시계

	ComPtr<ID3D11RasterizerState> NoCullNoClipRS;

	ComPtr<ID3D11SamplerState> LinearWrap;
	ComPtr<ID3D11SamplerState> LinearMipPointWrap;
	ComPtr<ID3D11SamplerState> LinearClamp;

	// DepthStencilState
	ComPtr<ID3D11DepthStencilState> DrawBasicDSS;
	ComPtr<ID3D11DepthStencilState> MaskDSS;
	ComPtr<ID3D11DepthStencilState> DrawMaskedDSS;

	// BlendState
	ComPtr<ID3D11BlendState> MirrorBlendState;
};