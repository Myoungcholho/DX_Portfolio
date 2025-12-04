#pragma once

class APlayer : public AActor
{
public:
	APlayer();

public:
	void Initialize() override;
	void Tick() override;

public:
	UStaticMeshComponent* GetStaticMeshComponent() { return mesh; }

private:
	UStaticMeshComponent* mesh;


private:
	bool bData = false;
};