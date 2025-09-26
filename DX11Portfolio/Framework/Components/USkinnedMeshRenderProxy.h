#pragma once

#include "Utilities/StructuredBuffer.h"

/// <summary>
/// 스냅샷된 데이터로 GPU자원을 관리하고 그리는 역할을 담당
/// </summary>
class USkinnedMeshRenderProxy : public URenderProxy
{
public:
	void Init(const vector<PBRMeshData>& meshData) override;

	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context) override;

public:
	void Draw(ID3D11DeviceContext* context) override;
	void DrawNormal(ID3D11DeviceContext* context) override;
	
	void InitBonesRT(int boneCount);
	void UpdateBonesGPU();
	void SetUpdateBones(const vector<Matrix>& bones);

private:
	StructuredBuffer<Matrix> boneBufferRT;
	int boneCount = 0;
};