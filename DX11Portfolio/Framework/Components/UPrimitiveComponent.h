#pragma once

#include "USceneComponent.h"
#include "Renders/RenderTypes.h"
//렌더링 가능한 모든 컴포넌트의 기반 클래스

class UPrimitiveComponent : public USceneComponent
{
public:
	WorldInvConstantBuffer& GetMeshConstants() { return m_meshConstsCPU; }
	MaterialConstants& GetMaterialConstants() { return m_materialConstsCPU; }
protected:
	bool bVisible = true;
	//vector<shared_ptr<Mesh>> m_meshes;				// 그릴 Mesh GPU 정보
	WorldInvConstantBuffer m_meshConstsCPU;			// World , InvTransform 행렬
	MaterialConstants m_materialConstsCPU;			// 메테리얼 관련 정보

protected:
	//ComPtr<ID3D11Buffer> m_meshConstsGPU;
	//ComPtr<ID3D11Buffer> m_materialConstsGPU;

public:
	void SetVisible(bool v) { bVisible = v; }

	virtual ERenderPass GetRenderPass() const { return ERenderPass::Opaque; }
public:
	virtual void Init() {}							// Primitive로부터 만들어진 virtual
	virtual void RefreshConstantsCPU();
	virtual void RenderNormal(ComPtr<ID3D11DeviceContext>& context) {}
public:
	//virtual void Render(ComPtr<ID3D11DeviceContext>& context) {}

	//virtual shared_ptr<URenderProxy> CreateRenderProxy() { return nullptr; }

public:
	//void SetDevice(ComPtr<ID3D11Device>& dev) { device = dev; }
	//void SetContext(ComPtr<ID3D11DeviceContext>& ctx) { context = ctx; }
	void SetPBRMeshData(const vector<PBRMeshData>& mesh) { m_meshData = mesh; }

public:
	shared_ptr<URenderProxy> virtual GetRenderProxy() = 0;

protected:
	//ComPtr<ID3D11Device> device;
	//ComPtr<ID3D11DeviceContext> context;
	vector<PBRMeshData> m_meshData;
};