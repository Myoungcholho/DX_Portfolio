#pragma once

#include <vector>

#include "Components/UPrimitiveComponent.h"
#include "Renders/RenderTypes.h"

class URenderQueue
{
public:
	// 프록시 추가 (소유권 이전)
	void AddProxy(std::shared_ptr<URenderProxy> proxy);

	// 프록시 목록 제공 (렌더링용)
	const std::vector<URenderProxy*>& GetSkyboxList() const { return m_skyboxList; }
	const std::vector<URenderProxy*>& GetOpaqueList() const { return m_opaqueList; }
	const std::vector<URenderProxy*>& GetTransparentList() const { return m_transparentList; }

	// 프레임 끝나고 초기화
	void Clear();

private:
	std::vector<std::shared_ptr<URenderProxy>> m_allProxies;

	std::vector<URenderProxy*> m_skyboxList;
	std::vector<URenderProxy*> m_opaqueList;
	std::vector<URenderProxy*> m_transparentList;
};