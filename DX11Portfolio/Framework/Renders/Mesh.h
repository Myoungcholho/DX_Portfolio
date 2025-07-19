#pragma once

#include "Framework.h"
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#include <d3d11.h>

struct FMesh
{
	ComPtr<ID3D11Buffer> VertexBuffer;
	ComPtr<ID3D11Buffer> IndexBuffer;

	ComPtr<ID3D11Buffer> VSCBuffer;
	ComPtr<ID3D11Buffer> PSCBuffer;

	std::shared_ptr<CTexture> Texture;

	UINT IndexCount = 0;
	UINT VertexCount = 0;
};

struct Mesh
{
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    UINT m_indexCount = 0;
    UINT m_vertexCount = 0;

    // ConstantBuffer -> 현재는 하나로 통합했지만 나중에 분리할 것
    ComPtr<ID3D11Buffer> vertexConstantBuffer;
    ComPtr<ID3D11Buffer> pixelConstantBuffer;

    // Texturing
    ComPtr<ID3D11Texture2D> albedoTexture;
    ComPtr<ID3D11Texture2D> emissiveTexture;
    ComPtr<ID3D11Texture2D> normalTexture;
    ComPtr<ID3D11Texture2D> heightTexture;
    ComPtr<ID3D11Texture2D> aoTexture;
    ComPtr<ID3D11Texture2D> metallicRoughnessTexture;

    ComPtr<ID3D11ShaderResourceView> albedoSRV;
    ComPtr<ID3D11ShaderResourceView> emissiveSRV;
    ComPtr<ID3D11ShaderResourceView> normalSRV;
    ComPtr<ID3D11ShaderResourceView> heightSRV;
    ComPtr<ID3D11ShaderResourceView> aoSRV;
    ComPtr<ID3D11ShaderResourceView> metallicRoughnessSRV;
};