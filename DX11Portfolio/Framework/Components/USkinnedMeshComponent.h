#pragma once

#include "UPrimitiveComponent.h"
#include "USkinnedMeshRenderProxy.h"
#include "Utilities/StructuredBuffer.h"
#include "UAnimInstance.h"

/// <summary>
/// 애니메이션 결과를 받아 그리기 직전까지 패키징하는 역할
/// 프록시, 스냅샷데이터, 애니메이션 데이터를 소유
/// </summary>
class USkinnedMeshComponent : public UPrimitiveComponent
{
public:
	USkinnedMeshComponent();
    ~USkinnedMeshComponent() override = default;

public:
    void Init() override;                           // 월드/InvTransform갱신
    void RefreshConstantsCPU() override;            // 월드/InvTransform갱신

    // 스키닝 데이터 세팅
    //void SetAssets(const vector<PBRMeshData>& meshes, shared_ptr<const AnimationData> anim);
    void SetAssets(shared_ptr<const CPUMeshAsset> meshes, shared_ptr<const AnimationData> anim);

    // 렌더러가 프록시를 가져갈 때
    shared_ptr<URenderProxy> GetRenderProxy() override;

    ERenderPass GetRenderPass() const override { return ERenderPass::Skinned; };

protected:
    shared_ptr<USkinnedMeshRenderProxy> renderProxy;
    shared_ptr<const AnimationData> AnimAsset;
    int boneCount;
    vector<Matrix> bonesSnapshot;
};