#pragma once

class APawnTest : public APawn
{
public:
	APawnTest();

public:
	void Initialize() override;

public:
	shared_ptr<USkeletalMeshComponent> GetSkeletalMeshComponent();

private:
	shared_ptr<USkeletalMeshComponent> skeletalMeshComponent;
};