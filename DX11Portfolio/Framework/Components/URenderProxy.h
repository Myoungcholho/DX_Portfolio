#pragma once

#include "Renders/RenderTypes.h"

class URenderProxy
{
public:
	URenderProxy();
	virtual ~URenderProxy() = default;
	virtual void Draw(ID3D11DeviceContext* context) = 0;

	ERenderPass renderPass = ERenderPass::Opaque;
	bool bVisible = true;

public:
	virtual void Init(const vector<PBRMeshData>& meshData) { }
	void SetMeshConstants(const WorldInvConstantBuffer& data) { m_meshConstsCPU = data; }
	void SetMaterialConstants(const MaterialConstants& data) { m_materialConstsCPU = data; }
	virtual void UpdateConstantBuffers(				// Primitive는 World, InVTransfom만 관리
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context) {}

protected:
	vector<shared_ptr<Mesh>> m_meshes;				// 그릴 Mesh GPU 정보

protected:
	WorldInvConstantBuffer m_meshConstsCPU;
	MaterialConstants m_materialConstsCPU;

protected:
	ComPtr<ID3D11Buffer> m_meshConstsGPU;
	ComPtr<ID3D11Buffer> m_materialConstsGPU;

protected:
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
};