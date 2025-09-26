#pragma once

#include <vector>

#include "Components/UPrimitiveComponent.h"
#include "Renders/RenderTypes.h"

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

	// 프레임 끝나고 초기화
	void Clear();

private:
	vector<shared_ptr<URenderProxy>> m_allProxies;

	vector<URenderProxy*> m_skyboxList;
	vector<URenderProxy*> m_opaqueList;
	vector<URenderProxy*> m_transparentList;
	vector<URenderProxy*> m_skinnedList;
};