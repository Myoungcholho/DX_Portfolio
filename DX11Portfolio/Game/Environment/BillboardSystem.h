#pragma once

#include "Systems/IExecutable.h"

#include <directxtk/SimpleMath.h>
#include <vector>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector4;

class BillboardSystem : public IExecutable
{
public:
	virtual void Initialize() override;
	virtual void Tick() override;
	virtual void UpdateGUI() override;
	virtual void Render() override;

private:
	vector<BillboardInstance*> BillboardInstances;
	BillboardRenderer_GS m_renderer;
	vector<BillboardPoint> m_points;
};