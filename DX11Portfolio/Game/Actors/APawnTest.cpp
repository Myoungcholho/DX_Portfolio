#include "Pch.h"
#include "APawnTest.h"
#include "Gameplay/ClassIDRegister.h"

REGISTER_CLASS(APawnTest);

APawnTest::APawnTest()
{
	m_name = "PawnTestActor";
	m_skeletalMeshComponent = make_shared<USkeletalMeshComponent>();
	root = m_skeletalMeshComponent;
	AddComponent(m_skeletalMeshComponent);

	shared_ptr<const CPUMeshAsset> asset = CPUAssetManager::GetProcedural("Rumy:character");
	shared_ptr<AnimationData> aniData = CPUAssetManager::GetAnimation("Rumy:Animation");

	m_skeletalMeshComponent->SetAssets(asset,aniData);
	m_skeletalMeshComponent->SetMaterialFactors(Vector3(1.0f), 0.8f, 0.0f);
	m_skeletalMeshComponent->SetRelativePosition(Vector3(0, -0.5f, -9.f));
	m_skeletalMeshComponent->SetTrack(0, true, 1.0f);

}

void APawnTest::Initialize()
{
	m_skeletalMeshComponent->Init();
}

shared_ptr<USkeletalMeshComponent> APawnTest::GetSkeletalMeshComponent()
{
	return m_skeletalMeshComponent;
}
