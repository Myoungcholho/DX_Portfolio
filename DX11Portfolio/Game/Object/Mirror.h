#pragma once

#include "Systems/IExecutable.h"

#include <directxtk/SimpleMath.h>
#include <vector>
//#include "MeshGroup.h"

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class Mirror : public IExecutable
{
public:
	Mirror(string name) {}

public:
	virtual void Initialize() {}
	virtual void Tick() {}
	virtual void UpdateGUI() {}
	virtual void Render() {}
	void UpdateConstantBuffer() {}

public:
	//shared_ptr<Transform> GetTransform() { return m_mirror->GetTransform(); }

private:
	shared_ptr<Model> m_mirror;

private:
	Plane m_mirrorPlane;
	float m_mirrorAlpha = 0.5f;
	
private:
	Matrix reflectionRow;

private:
	string Name;
};