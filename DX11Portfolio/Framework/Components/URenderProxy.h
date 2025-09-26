#pragma once

#include "Renders/RenderTypes.h"

class URenderProxy
{
public:
	URenderProxy();
	virtual ~URenderProxy() = default;
	virtual void Draw(ID3D11DeviceContext* context) = 0;
	virtual void DrawNormal(ID3D11DeviceContext* context) {}

	ERenderPass renderPass = ERenderPass::Opaque;
	bool bVisible = true;
	bool drawNormal = false;
	bool castShadow = true;

public:
	void SetDrawNormal(bool drawNormal) { drawNormal = drawNormal; }

public:
	virtual void Init(const vector<PBRMeshData>& meshData) { }
	//virtual void Init(const vector<PBRMeshData>& meshData, const AnimationData& aniData) {}
	void SetMeshConstants(const WorldInvConstantBuffer& data) { meshConstsCPU = data; }
	void SetMaterialConstants(const MaterialConstants& data) { materialConstsCPU = data; }
	virtual void UpdateConstantBuffers(				// Primitive는 World, InVTransfom만 관리
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context) {}

	MaterialConstants GetMaterialConstants() const { return materialConstsCPU; }

protected:
	vector<shared_ptr<Mesh>> meshes;				// 그릴 Mesh GPU 정보

protected:
	WorldInvConstantBuffer meshConstsCPU;
	MaterialConstants materialConstsCPU;

protected:
	ComPtr<ID3D11Buffer> meshConstsGPU;
	ComPtr<ID3D11Buffer> materialConstsGPU;

protected:
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
};