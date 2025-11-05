#include "Framework.h"
#include "URenderQueue.h"

void URenderQueue::AddProxy(std::shared_ptr<URenderProxy> proxy)
{
    if (!proxy)
        return;

    // 소유권 이전
    m_allProxies.push_back(std::move(proxy));
    URenderProxy* raw = m_allProxies.back().get();

    // 분류
    switch (raw->renderPass)
    {
    case ERenderPass::Skybox:
        m_skyboxList.push_back(raw);
        break;
    case ERenderPass::Opaque:
    {
        //m_opaqueList.push_back(raw);

        if (auto* staticProxy = dynamic_cast<UStaticMeshRenderProxy*>(raw))
        {
            auto mesh = staticProxy->GetMeshAsset();
            if (mesh)
            {
                InstanceData data;
                data.meshConstsCPU = staticProxy->GetWorldInvConstants();
                data.materialConstsCPU = staticProxy->GetMaterialConstants();

                m_opaqueInstanceBatches[mesh].mesh = mesh;
                m_opaqueInstanceBatches[mesh].instances.push_back(data);
            }
        }
        
        break;
    }
    case ERenderPass::Skinned:
    {
        //m_skinnedList.push_back(raw);

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

                m_skinnedInstanceBatches[mesh].mesh = mesh;
                m_skinnedInstanceBatches[mesh].instances.push_back(move(data));
                m_skinnedInstanceBatches[mesh].maxBoneCount = max(m_skinnedInstanceBatches[mesh].maxBoneCount, boneCount);
            }
        }

        break;
    }
    }
}

void URenderQueue::Clear()
{
    m_skyboxList.clear();
    m_opaqueList.clear();
    m_transparentList.clear();
    m_skinnedList.clear();

    m_opaqueInstanceBatches.clear();
    m_skinnedInstanceBatches.clear();
}