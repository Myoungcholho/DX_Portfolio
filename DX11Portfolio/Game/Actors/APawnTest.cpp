#include "Pch.h"
#include "APawnTest.h"
#include "Gameplay/ClassIDRegister.h"

REGISTER_CLASS(APawnTest);

APawnTest::APawnTest()
{
	name = "PawnTestActor";
	skeletalMeshComponent = make_shared<USkeletalMeshComponent>();
	root = skeletalMeshComponent;
	AddComponent(skeletalMeshComponent);

	shared_ptr<const CPUMeshAsset> asset = CPUAssetManager::GetProcedural("Rumy:character");
	shared_ptr<AnimationData> aniData = CPUAssetManager::GetAnimation("Rumy:Animation");

	skeletalMeshComponent->SetAssets(asset,aniData);
	skeletalMeshComponent->SetMaterialFactors(Vector3(1.0f), 0.8f, 0.0f);
	skeletalMeshComponent->SetRelativePosition(Vector3(0, -0.5f, -9.f));
	skeletalMeshComponent->SetTrack(0, true, 1.0f);

}

void APawnTest::Initialize()
{
	skeletalMeshComponent->Init();
}

shared_ptr<USkeletalMeshComponent> APawnTest::GetSkeletalMeshComponent()
{
	return skeletalMeshComponent;
}
