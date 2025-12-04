#include "Pch.h"
#include "APawnTest.h"
#include "Gameplay/ClassIDRegister.h"

REGISTER_CLASS(APawnTest);

APawnTest::APawnTest()
{
	SetName("PawnTestActor");

	// 1) 루트 + 메쉬
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalComponent");
	SetRootComponent(mesh);

	shared_ptr<const CPUMeshAsset> asset = CPUAssetManager::GetProcedural("Rumy:character");
	shared_ptr<AnimationData> aniData = CPUAssetManager::GetAnimation("Rumy:Animation");

	mesh->SetAssets(asset,aniData);
	mesh->SetMaterialFactors(albedo, roughnessFactor, metaliicFactor);
	mesh->SetRelativePosition(InitPos);
	mesh->SetTrack(0, true, 1.0f);

	// 2) 라이트를 메쉬에 어태치
	attachTestlight = CreateDefaultSubobject<ULightComponent>("LightComponent");
	attachTestlight->AttachTo(mesh, EAttachMode::KeepRelative);
}

void APawnTest::Initialize()
{
	APawn::Initialize();

	mesh->Init();

	// light 초기값 설정
	attachTestlight->SetRadiance(Vector3(5.0f));
	attachTestlight->SetFalloff(0.0f, 6.0f);
	attachTestlight->SetLightType((uint32_t)LIGHT_POINT | LIGHT_SHADOW);
	attachTestlight->SetRelativePosition(Vector3(0, 0, 0));
	attachTestlight->SetRelativeRotation(Vector3(0, 0, 0));
}

void APawnTest::Tick()
{
	APawn::Tick();

	CKeyboard* key = CKeyboard::Get();

	if (key == nullptr)
		return;

	if (key->Press(VK_LSHIFT))
		MoveSpeed = 2.0f;
	else
		MoveSpeed = 1.0f;
}
