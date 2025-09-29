#include "Framework.h"
#include "USkinnedMeshComponent.h"

// 본행렬 평가 + 커맨드 발행

USkinnedMeshComponent::USkinnedMeshComponent()
{
	mName = "SkinnedMeshComponent";
}

void USkinnedMeshComponent::Init()
{
	renderProxy = make_shared<USkinnedMeshRenderProxy>();

	renderProxy->Init(meshData);
	renderProxy->renderPass = GetRenderPass();
	renderProxy->bVisible = bVisible;

	materialConstsCPU = renderProxy->GetMaterialConstants();

	// GPU 버퍼 생성/연결은 렌더스레드(프록시)에서 수행
	boneCount = (int)AnimAsset->boneParents.size();
	if (boneCount <= 0) boneCount = 1;
	bonesSnapshot.assign(boneCount, Matrix());

	renderProxy->InitBonesRT(boneCount);
}

void USkinnedMeshComponent::RefreshConstantsCPU()
{
	// 월드/역전치 갱신
	UPrimitiveComponent::RefreshConstantsCPU();
}

//void USkinnedMeshComponent::SetAssets(const vector<PBRMeshData>& meshes, shared_ptr<const AnimationData> anim)
//{
//	meshData = meshes;
//	AnimAsset = anim;
//}

void USkinnedMeshComponent::SetAssets(shared_ptr<const CPUMeshAsset> meshes, shared_ptr<const AnimationData> anim)
{
	meshData = meshes;
	AnimAsset = anim;
}


shared_ptr<URenderProxy> USkinnedMeshComponent::GetRenderProxy()
{
	RefreshConstantsCPU();									// CPU데이터 스냅샷에 동기화

	renderProxy->SetMeshConstants(meshConstsCPU);
	renderProxy->SetMaterialConstants(materialConstsCPU);
	renderProxy->SetDrawNormal(drawNormals);
	renderProxy->SetUpdateBones(bonesSnapshot);

	return renderProxy;
}