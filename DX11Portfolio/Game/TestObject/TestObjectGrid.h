#pragma once

#include "Systems/IExecutable.h"

#include <directxtk/SimpleMath.h>
#include <vector>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class TestObjectGrid : public IExecutable
{
public:
	virtual void Initialize();
	virtual void Destroy();

	virtual void UpdateGUI();
	virtual void Tick();
	virtual void Render();

private:
	auto MakeGrid(const float width, const float height, const int numSlice, const int numStacks);

private:
	ComPtr<ID3D11VertexShader> VertexShader;
	ComPtr<ID3D11PixelShader> PixelShader;
	ComPtr<ID3D11InputLayout> InputLayout;

	// 정점/인덱스/상수버퍼
	FMesh Mesh;


	WorldInvConstantBuffer ConstantBufferData;

	CTexture* texture;
};