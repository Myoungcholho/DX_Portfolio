#pragma once

class ASkinnedTestActor : public AActor
{
public:
	ASkinnedTestActor();

public:
	void Initialize() override;

public:
	USkeletalMeshComponent* GetSkeletalMeshComponent() { return skMesh; }

private:
	USkeletalMeshComponent* skMesh;
};	