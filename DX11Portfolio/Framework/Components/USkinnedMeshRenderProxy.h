#pragma once

#include "Utilities/StructuredBuffer.h"

/// <summary>
/// 스냅샷된 데이터로 GPU자원을 관리하고 그리는 역할을 담당
/// </summary>
class USkinnedMeshRenderProxy : public URenderProxy
{
public:
	void Init(shared_ptr<const CPUMeshAsset> asset) override;

	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context) override;

public:
	void Draw(ID3D11DeviceContext* context) override;
	void DrawNormal(ID3D11DeviceContext* context) override;
	
	// DrawIndexed용
	void InitBonesRT(int boneCount);
	void UpdateBonesGPU();
	void SetUpdateBones(const vector<Matrix>& bones);

	// 인스턴싱
	void SetUpdateBonesCPU(const vector<Matrix>& bones);
	vector<Matrix>* GetBonesPalette()
	{
		return &bonesPalette;
	}

private:
	StructuredBuffer<Matrix> boneBufferRT;		// 미사용

	vector<Matrix> bonesPalette;
	int boneCount = 0;
};