#include "Pch.h"
#include "ASkinnedTestActor.h"

ASkinnedTestActor::ASkinnedTestActor()
{
	name = "SkinnedTest";
	skeletalMeshComponent = make_shared<USkeletalMeshComponent>();
	root = skeletalMeshComponent;
	AddComponent(skeletalMeshComponent);
}

void ASkinnedTestActor::Initialize()
{
	skeletalMeshComponent->Init();

}

shared_ptr<USkeletalMeshComponent> ASkinnedTestActor::GetSkeletalMeshComponent()
{
	return skeletalMeshComponent;
}
