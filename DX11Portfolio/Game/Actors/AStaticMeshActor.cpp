#include "Pch.h"
#include "AStaticMeshActor.h"

AStaticMeshActor::AStaticMeshActor()
{
	name = "Plane";
	staticMeshComponent = make_shared<UStaticMeshComponent>();
	SetRootComponent(staticMeshComponent);
	AddComponent(staticMeshComponent);

	lightComponent = make_shared<ULightComponent>();
	AddComponent(lightComponent);

	//lightComponent2 = make_shared<ULightComponent>();
	//AddComponent(lightComponent2);
	//lightComponent2->SetName("Light2");

	//lightComponent3 = make_shared<ULightComponent>();
	//AddComponent(lightComponent3);
	//lightComponent3->AttachTo(lightComponent2.get(),EAttachMode::KeepWorld);
	//lightComponent3->SetName("Light3");
}

AStaticMeshActor::AStaticMeshActor(shared_ptr<const CPUMeshAsset> InAsset)
{
	name = "Plane";
	staticMeshComponent = make_shared<UStaticMeshComponent>();
	SetRootComponent(staticMeshComponent);
	AddComponent(staticMeshComponent);
	staticMeshComponent->SetPBRMeshData(InAsset);

	lightComponent = make_shared<ULightComponent>();
	AddComponent(lightComponent);

	//lightComponent2 = make_shared<ULightComponent>();
	//AddComponent(lightComponent2);
	//lightComponent2->SetName("Light2");

	//lightComponent3 = make_shared<ULightComponent>();
	//AddComponent(lightComponent3);
	//lightComponent3->AttachTo(lightComponent3.get(), EAttachMode::KeepWorld);
	//lightComponent3->SetName("Light3");
}

void AStaticMeshActor::Initialize()
{
	// 프록시 생성
	staticMeshComponent->Init();

	// light 초기값 설정
	lightComponent->SetRadiance(Vector3(5.0f));
	lightComponent->SetFalloff(0.0f, 6.0f);
	lightComponent->SetLightType((uint32_t)LIGHT_POINT | LIGHT_SHADOW);
	lightComponent->SetRelativePosition(Vector3(1, -10.0f, -2.0f));
	lightComponent->SetRelativeRotation(Vector3(180, 0, 0));
}