#pragma once

class AStaticMeshActor : public AActor
{
public:
	AStaticMeshActor();

public:
	void Initialize() override;

public:
	shared_ptr<UStaticMeshComponent> GetStaticMeshComponent();

private:
	shared_ptr<UStaticMeshComponent> m_staticMeshComponent;
};