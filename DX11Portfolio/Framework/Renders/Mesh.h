#pragma once

#include "Framework.h"

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