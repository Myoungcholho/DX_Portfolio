#pragma once

class AStaticMeshActor : public AActor
{
public:
	AStaticMeshActor();
	AStaticMeshActor(shared_ptr<const CPUMeshAsset> InAsset);

public:
	void Initialize() override;

public:
	UStaticMeshComponent* GetStaticMeshComponent() { return staticMeshComponent; }
	ULightComponent* GetLightComponent() { return lightComponent; }

private:
	UStaticMeshComponent* staticMeshComponent;
	ULightComponent* lightComponent;
};