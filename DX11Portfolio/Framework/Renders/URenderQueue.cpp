#include "Framework.h"
#include "URenderQueue.h"

void URenderQueue::AddProxy(std::shared_ptr<URenderProxy> proxy)
{
    if (!proxy)
        return;

    // 소유권 이전
    allProxies.push_back(std::move(proxy));
    URenderProxy* raw = allProxies.back().get();

    // 분류
    switch (raw->renderPass)
    {
    case ERenderPass::Skybox:
        skyboxList.push_back(raw);
        break;
    case ERenderPass::Opaque:
    {
        //opaqueList.push_back(raw);

        if (auto* staticProxy = dynamic_cast<UStaticMeshRenderProxy*>(raw))
        {
            auto mesh = staticProxy->GetMeshAsset();
            if (mesh)
            {
                InstanceData data;
                data.meshConstsCPU = staticProxy->GetWorldInvConstants();
                data.materialConstsCPU = staticProxy->GetMaterialConstants();

                opaqueInstanceBatches[mesh].mesh = mesh;
                opaqueInstanceBatches[mesh].instances.push_back(data);
            }
        }
        
        break;
    }
    case ERenderPass::Skinned:
    {
        //skinnedList.push_back(raw);

        if (auto* skinnedProxy = dynamic_cast<USkinnedMeshRenderProxy*>(raw))
        {
            auto mesh = skinnedProxy->GetMeshAsset();
            if (mesh)
            {
                SkinnedInstanceData data;
                data.meshConstsCPU = skinnedProxy->GetWorldInvConstants();
                data.materialConstsCPU = skinnedProxy->GetMaterialConstants();
                data.bonePaletteCPU = skinnedProxy->GetBonesPalette();

                uint32_t boneCount = static_cast<uint32_t>(data.bonePaletteCPU->size());

                skinnedInstanceBatches[mesh].mesh = mesh;
                skinnedInstanceBatches[mesh].instances.push_back(move(data));
                skinnedInstanceBatches[mesh].maxBoneCount = max(skinnedInstanceBatches[mesh].maxBoneCount, boneCount);
            }
        }

        break;
    }
    }
}

void URenderQueue::Clear()
{
    skyboxList.clear();
    opaqueList.clear();
    transparentList.clear();
    skinnedList.clear();

    opaqueInstanceBatches.clear();
    skinnedInstanceBatches.clear();
}