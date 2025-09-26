#pragma once

class AStaticMeshActor : public AActor
{
public:
	AStaticMeshActor();
	AStaticMeshActor(const vector<PBRMeshData>& InData);

public:
	void Initialize() override;

public:
	shared_ptr<UStaticMeshComponent> GetStaticMeshComponent() { return m_staticMeshComponent; }
	shared_ptr<ULightComponent> GetLightComponent() { return m_lightComponent; }

private:
	shared_ptr<UStaticMeshComponent> m_staticMeshComponent;
	shared_ptr<ULightComponent> m_lightComponent;
	shared_ptr<ULightComponent> m_lightComponent2;
	shared_ptr<ULightComponent> m_lightComponent3;
};