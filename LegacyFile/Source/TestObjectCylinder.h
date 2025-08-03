#pragma once

#include "Systems/IExecutable.h"

#include <directxtk/SimpleMath.h>
#include <vector>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class TestObjectCylinder : public IExecutable
{
public:
	auto MakeCylinder(const float bottomRadius, const float topRadius, float height, int sliceCount);

public:
	virtual void Initialize();
	virtual void Destroy();

	virtual void UpdateGUI();
	virtual void Tick();
	virtual void Render();

private:
	ComPtr<ID3D11VertexShader> VertexShader;
	ComPtr<ID3D11PixelShader> PixelShader;
	ComPtr<ID3D11InputLayout> InputLayout;

	FMesh Mesh;
	WorldInvConstantBuffer ConstantBufferData;
	CTexture* texture;
};