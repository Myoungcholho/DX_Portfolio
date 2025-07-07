#pragma once

#include "Systems/IExecutable.h"

#include <directxtk/SimpleMath.h>
#include <vector>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class TextureGround : public IExecutable
{
public:
	virtual void Initialize();
	virtual void Destroy();

	virtual void Tick();
	virtual void UpdateGUI();
	virtual void Render();

private:
	// Shader
	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11HullShader> m_hullShader;
	ComPtr<ID3D11DomainShader> m_domainShader;
	ComPtr<ID3D11GeometryShader> m_geometryShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;

private:
	// ConstantBufferData
	WorldInvConstantBuffer worldInvConstantBufferData;
	HeightMapConstantBuffer heightMapConstantBufferData;
	CameraConstantBuffer cameraConstantBufferData;
	MaterialConstantBuffer materialConstantBufferData;
	LightConstantBuffer lightConstantBufferData;
	RenderOptionsConstantBuffer renderOptionsConstantBufferData;

	// 추가로 보내고 있는 것
	// 1. View Proj
	// 2. IBL SRV 2장
	// 3. Sampler

	// ConstnatBuffer
	ComPtr<ID3D11Buffer> worldInvConstantBuffer;
	ComPtr<ID3D11Buffer> heightMapConstantBuffer;
	ComPtr<ID3D11Buffer> cameraConstantBuffer;
	ComPtr<ID3D11Buffer> materialConstantBuffer;
	ComPtr<ID3D11Buffer> lightConstantBuffer;
	ComPtr<ID3D11Buffer> renderOptionsConstantBuffer;

private:
	// (Vertex & Index) Buffer
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	UINT m_indexCount = 0;
	UINT m_vertexCount = 0;

	// Texture
	ComPtr<ID3D11Texture2D> albedoTexture;
	ComPtr<ID3D11Texture2D> normalTexture;
	ComPtr<ID3D11Texture2D> heightTexture;
	ComPtr<ID3D11Texture2D> aoTexture;
	ComPtr<ID3D11ShaderResourceView> albedoTextureResourceView;
	ComPtr<ID3D11ShaderResourceView> normalTextureResourceView;
	ComPtr<ID3D11ShaderResourceView> heightTextureResourceView;
	ComPtr<ID3D11ShaderResourceView> aoTextureResourceView;

private:
	// InputLayout
	ComPtr<ID3D11InputLayout> m_inputLayout;


private:
	shared_ptr<Transform> transform;
};