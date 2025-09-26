#include "Pch.h"
#include "AStaticMeshActor.h"

AStaticMeshActor::AStaticMeshActor()
{
	m_name = "Plane";
	m_staticMeshComponent = make_shared<UStaticMeshComponent>();
	SetRootComponent(m_staticMeshComponent);
	AddComponent(m_staticMeshComponent);

	m_lightComponent = make_shared<ULightComponent>();
	AddComponent(m_lightComponent);

	m_lightComponent2 = make_shared<ULightComponent>();
	AddComponent(m_lightComponent2);
	m_lightComponent2->SetName("Light2");

	m_lightComponent3 = make_shared<ULightComponent>();
	AddComponent(m_lightComponent3);
	m_lightComponent3->AttachTo(m_lightComponent2.get(),EAttachMode::KeepWorld);
	m_lightComponent3->SetName("Light3");
}

AStaticMeshActor::AStaticMeshActor(const vector<PBRMeshData>& InData)
{
	m_name = "Plane";
	m_staticMeshComponent = make_shared<UStaticMeshComponent>();
	SetRootComponent(m_staticMeshComponent);
	AddComponent(m_staticMeshComponent);
	m_staticMeshComponent->SetPBRMeshData(InData);

	m_lightComponent = make_shared<ULightComponent>();
	AddComponent(m_lightComponent);

	m_lightComponent2 = make_shared<ULightComponent>();
	AddComponent(m_lightComponent2);
	m_lightComponent2->SetName("Light2");

	m_lightComponent3 = make_shared<ULightComponent>();
	AddComponent(m_lightComponent3);
	m_lightComponent3->AttachTo(m_lightComponent2.get(), EAttachMode::KeepWorld);
	m_lightComponent3->SetName("Light3");
}

void AStaticMeshActor::Initialize()
{
	// 프록시 생성
	m_staticMeshComponent->Init();

	// light 초기값 설정
	m_lightComponent->SetRadiance(Vector3(5.0f));
	m_lightComponent->SetFalloff(0.0f, 6.0f);
	m_lightComponent->SetLightType((uint32_t)LIGHT_POINT | LIGHT_SHADOW);
	m_lightComponent->SetRelativePosition(Vector3(1, -10.0f, -2.0f));
	m_lightComponent->SetRelativeRotation(Vector3(180, 0, 0));
}