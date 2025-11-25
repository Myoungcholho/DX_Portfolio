#pragma once

class APlayer : public AActor
{
public:
	APlayer();

public:
	void Initialize() override;
	void Tick() override;

public:
	shared_ptr<UStaticMeshComponent> GetStaticMeshComponent();

private:
	shared_ptr<UStaticMeshComponent> staticMeshComponent;


private:
	bool bData = false;
};