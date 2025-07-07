#pragma once

#include "Systems/IExecutable.h"

#include <directxtk/SimpleMath.h>
#include <vector>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class TestObjectQuad : public IExecutable
{
public:
	virtual void Initialize();
	virtual void Destroy();

	virtual void Tick();
	virtual void UpdateGUI();
	virtual void Render();

private:
	struct TessellatedQuadConstantData
	{
		Vector3 eyeWorld;
		float padding;
		Matrix model;
	};

	static_assert((sizeof(TessellatedQuadConstantData) % 16) == 0,
		"Constant Buffer size must be 16-byte aligned");
private:
	TessellatedQuadConstantData m_constantData;
	ComPtr<ID3D11Buffer> m_constantBuffer;

private:
	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11HullShader> m_hullShader;
	ComPtr<ID3D11DomainShader> m_domainShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;

private:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11InputLayout> m_inputLayout;

private:
	uint32_t m_indexCount = 0;

private:
	shared_ptr<Transform> transform;
};

