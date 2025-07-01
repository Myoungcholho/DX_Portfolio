#pragma once

class GraphicsDevice
{
public:
	static GraphicsDevice* Get();

public:
	static void Create();
	static void Destory();

public:
	// RS
	/*ID3D11RasterizerState* GetSolidRS() const;
	ID3D11RasterizerState* GetWireFrameRS() const;
	ID3D11RasterizerState* GetNoCullNoClipRS() const;*/

	// SamplerState
	ID3D11SamplerState* const* GetLinearWrapSampler() const;
	ID3D11SamplerState* const* GetLinearClampSampler() const;

public:
	// RSState Àû¿ë
	void ApplySolidRasterizer();
	void ApplyWireframeRasterizer();
	void ApplyNoCullNoClipRasterizer();

private:
	GraphicsDevice();
	~GraphicsDevice();

private:
	void CreateBasicRasterizerState();
	void CreateWireRasterizerState();
	void CreateNoCullNoClipRasterizerState();

	void CreateLinearWrapSamplerState();
	void CreateLinearClampSamplerState();

private:
	static GraphicsDevice* Instance;

private:
	ComPtr<ID3D11RasterizerState> SolidRS;
	ComPtr<ID3D11RasterizerState> WireFrameRS;
	ComPtr<ID3D11RasterizerState> NoCullNoClipRS;

	ComPtr<ID3D11SamplerState> LinearWrap;
	ComPtr<ID3D11SamplerState> LinearClamp;

};