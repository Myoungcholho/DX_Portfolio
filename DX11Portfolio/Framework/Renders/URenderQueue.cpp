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
        m_opaqueList.push_back(raw);
        break;
    }
}

void URenderQueue::Clear()
{
    m_skyboxList.clear();
    m_opaqueList.clear();
    m_transparentList.clear();
}