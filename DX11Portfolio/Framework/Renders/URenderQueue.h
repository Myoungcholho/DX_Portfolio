#pragma once

#include <vector>

#include "Components/UPrimitiveComponent.h"
#include "Renders/RenderTypes.h"

// 스테틱 메시 인스턴스 데이터
struct InstanceData
{
	WorldInvConstantBuffer meshConstsCPU;
	MaterialConstants materialConstsCPU;
};

struct InstanceBatch
{
	shared_ptr<const GPUMeshAsset> mesh;			// 공유하는 GPU 메쉬
	vector<InstanceData> instances;					// 인스턴스 데이터들
};

// 스킨드 메시 인스턴스 데이터
struct SkinnedInstanceData
{
	WorldInvConstantBuffer meshConstsCPU;		// WorldMatrix
	MaterialConstants materialConstsCPU;		// Material params
	vector<Matrix>* bonePaletteCPU;				// Bone matrices
};

struct SkinnedInstanceBatch
{
	shared_ptr<const GPUMeshAsset> mesh;		// Shared skinned mesh
	vector<SkinnedInstanceData> instances;		// Per-instance data
	uint32_t maxBoneCount = 0;					
};

// 스트럭쳐드 버퍼로 보내기 위한 구조체
struct SkinnedInstanceDataGPU
{
	WorldInvConstantBuffer meshConstsCPU;
	MaterialConstants materialConstsCPU;
};

struct SkinnedBatchCB
{
	UINT g_maxBoneCount;
	UINT padding[3];
};

class URenderQueue
{
public:
	// 프록시 추가 (소유권 이전)
	void AddProxy(shared_ptr<URenderProxy> proxy);

	// 프록시 목록 제공 (렌더링용)
	const vector<URenderProxy*>& GetSkyboxList() const { return m_skyboxList; }
	const vector<URenderProxy*>& GetOpaqueList() const { return m_opaqueList; }
	const vector<URenderProxy*>& GetTransparentList() const { return m_transparentList; }
	const vector<URenderProxy*>& GetSkinnedList() const { return m_skinnedList; }

	// 인스턴싱용
	const unordered_map<shared_ptr<const GPUMeshAsset>, InstanceBatch>& GetOpaqueInstanceBatches() const 
	{
		return m_opaqueInstanceBatches;
	}
	const unordered_map<shared_ptr<const GPUMeshAsset>, SkinnedInstanceBatch>& GetSkinnedInstanceBatches() const
	{
		return m_skinnedInstanceBatches;
	}

	// 프레임 끝나고 초기화
	void Clear();

private:
	vector<shared_ptr<URenderProxy>> m_allProxies;

	vector<URenderProxy*> m_skyboxList;
	vector<URenderProxy*> m_opaqueList;
	vector<URenderProxy*> m_transparentList;
	vector<URenderProxy*> m_skinnedList;

	unordered_map<shared_ptr<const GPUMeshAsset>, InstanceBatch> m_opaqueInstanceBatches;
	unordered_map<shared_ptr<const GPUMeshAsset>, SkinnedInstanceBatch> m_skinnedInstanceBatches;
};