#pragma once

#include <vector>

#include "Components/UPrimitiveComponent.h"
#include "Renders/RenderTypes.h"

class URenderQueue
{
public:
	void Add(UPrimitiveComponent* comp);
	void Clear();

public:
	// 읽기 전용으로 RETURN
	const vector<UPrimitiveComponent*>& GetOpaqueList() const { return opaqueList; }
	const vector<UPrimitiveComponent*>& GetSkyboxList() const { return skyboxList; }
	const vector<UPrimitiveComponent*>& GetMirrorList() const { return mirrorList; }

private:
	std::vector<UPrimitiveComponent*> opaqueList;
	std::vector<UPrimitiveComponent*> skyboxList;
	std::vector<UPrimitiveComponent*> mirrorList;
};