#pragma once

class ASkinnedTestActor : public AActor
{
public:
	ASkinnedTestActor();

public:
	void Initialize() override;

public:
	shared_ptr<USkeletalMeshComponent> GetSkeletalMeshComponent();

private:
	shared_ptr<USkeletalMeshComponent> skeletalMeshComponent;
};	