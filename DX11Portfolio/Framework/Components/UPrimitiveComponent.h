#pragma once

#include "USceneComponent.h"
#include "Renders/RenderTypes.h"

//렌더링 가능한 모든 컴포넌트의 기반 클래스
class UPrimitiveComponent : public USceneComponent
{
public:
	~UPrimitiveComponent() override = default;

public:
	bool drawNormals = false;
public:
	void SetMaterialFactors(const Vector3& albedo, float roughness, float metallic)
	{
		materialConstsCPU.albedoFactor = albedo;
		materialConstsCPU.roughnessFactor = roughness;
		materialConstsCPU.metallicFactor = metallic;
	}

public:
	const WorldInvConstantBuffer& GetMeshConstants() const { return meshConstsCPU; }
	const MaterialConstants& GetMaterialConstants() const { return materialConstsCPU; }
	bool GetNormalDraws() { return drawNormals; }

	void SetMeshConstants(WorldInvConstantBuffer& Indata) { meshConstsCPU = Indata; }
	void SetMaterialConstants(MaterialConstants& Indata) { materialConstsCPU = Indata; }
	void SetNormalDraws(bool InValue) { drawNormals = InValue; }
protected:
	bool bVisible = true;
	
	WorldInvConstantBuffer meshConstsCPU;			// World , InvTransform 행렬
	MaterialConstants materialConstsCPU;			// 메테리얼 관련 정보

public:
	void SetVisible(bool v) { bVisible = v; }

	virtual ERenderPass GetRenderPass() const { return ERenderPass::Opaque; }
public:
	virtual void Init() {}							// Primitive로부터 만들어진 virtual
	virtual void RefreshConstantsCPU();
	virtual void RenderNormal(ComPtr<ID3D11DeviceContext>& context) {}


public:
	shared_ptr<URenderProxy> virtual GetRenderProxy() = 0;

public:
	void SetPBRMeshData(shared_ptr<const CPUMeshAsset>  mesh) { meshData = mesh; }
	shared_ptr<const CPUMeshAsset> GetPBRMeshData() const { return meshData; }
protected:
	shared_ptr<const CPUMeshAsset> meshData;
};