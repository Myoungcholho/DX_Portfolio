#pragma once

#include "Systems/IExecutable.h"

class ImageFilter // : public IExecutable
{
public:
	ImageFilter(const wstring vertexPrefix, const wstring pixelPrefix);
	ImageFilter(const wstring vertexPrefix, const wstring pixelPrefix, int width, int height);

public:
	void Initialize();
	void Destroy();

	void UpdateGUI();
	void Tick();
	void Render();
	
public:
	void SetShaderResources(const vector<ComPtr<ID3D11ShaderResourceView>>& resources);
	void SetRenderTargets(const vector<ComPtr<ID3D11RenderTargetView>>& targets);

public:
	struct SamplingPixelConstantData 
	{
		float dx;
		float dy;
		float threshold;
		float strength;
		Vector4 options;
	};

public:
	SamplingPixelConstantData m_pixelConstData;

	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;

private:
	void ReSizeScreen();

private:
	// 생성자 정보 멤버 저장
	wstring m_vertexPrefix;
	wstring m_pixelPrefix;
	int m_width;
	int m_height;

	shared_ptr<FMesh> m_mesh;
	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11InputLayout> m_inputLayout;
	
	D3D11_VIEWPORT m_viewport;

	vector<ID3D11ShaderResourceView*> m_shaderResources;
	vector<ID3D11RenderTargetView*> m_renderTargets;
};