#include "Framework.h"
#include "URenderQueue.h"

void URenderQueue::Add(UPrimitiveComponent* comp)
{
	switch (comp->GetRenderPass())
	{
	case ERenderPass::Opaque:
		opaqueList.push_back(comp);
		break;
	case ERenderPass::Skybox:
		skyboxList.push_back(comp);
		break;
	case ERenderPass::Transparent:

		break;
	case ERenderPass::Shadow:

		break;
	}
}

void URenderQueue::Clear()
{
	opaqueList.clear();
	mirrorList.clear();
	skyboxList.clear();
}
