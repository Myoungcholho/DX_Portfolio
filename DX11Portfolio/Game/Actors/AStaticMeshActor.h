#pragma once

class AStaticMeshActor : public AActor
{
public:
	AStaticMeshActor();
	AStaticMeshActor(shared_ptr<const CPUMeshAsset> InAsset);

public:
	void Initialize() override;

public:
	shared_ptr<UStaticMeshComponent> GetStaticMeshComponent() { return staticMeshComponent; }
	shared_ptr<ULightComponent> GetLightComponent() { return lightComponent; }

private:
	shared_ptr<UStaticMeshComponent> staticMeshComponent;
	shared_ptr<ULightComponent> lightComponent;
};