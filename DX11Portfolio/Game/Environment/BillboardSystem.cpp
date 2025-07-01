#include "Pch.h"
#include "BillboardSystem.h"

void BillboardSystem::Initialize()
{
	vector<BillboardPoint> points;
	Vector4 p = { 0.0f,0.0f,0.0f,1.0f };
	points.push_back({ p });

	Vector3 pos = Vector3(-6.5f, -4.f, 16.5f);
	float dx = 1.5f;

	for (int i = 0; i < 10; ++i)
	{
		BillboardInstance* data = new BillboardInstance();

		data->Initialize(points);

		data->transform->SetPosition(pos);
		data->transform->SetRotation(Vector3(0.f, 0.f, 0.f));

		BillboardInstances.push_back(data);
		pos.x += dx;
	}
	m_renderer.Initialize();
}

void BillboardSystem::Tick()
{
	for (BillboardInstance* d : BillboardInstances)
		d->Tick();
	m_renderer.Tick();
}

void BillboardSystem::UpdateGUI()
{
	for (BillboardInstance* d : BillboardInstances)
		d->UpdateGUI();
}

void BillboardSystem::Render()
{
	for (BillboardInstance* d : BillboardInstances)
	{
		d->Render();
		m_renderer.Render(d);
	}
}