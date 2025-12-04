#include "Pch.h"
#include "ASkinnedTestActor.h"

ASkinnedTestActor::ASkinnedTestActor()
{
	SetName("SkinnedTest");

	/*skMesh = make_shared<USkeletalMeshComponent>();
	rootShared = skeletalMeshComponent;
	AddComponent(skeletalMeshComponent);*/

	skMesh = CreateDefaultSubobject<USkeletalMeshComponent>("skeletal");
	SetRootComponent(skMesh);
}

void ASkinnedTestActor::Initialize()
{
	skMesh->Init();

}