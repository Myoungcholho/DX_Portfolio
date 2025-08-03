#include "Pch.h"
#include "AStaticMeshActor.h"

AStaticMeshActor::AStaticMeshActor()
{
	m_staticMeshComponent = make_shared<UStaticMeshComponent>();
	root = m_staticMeshComponent;
	AddComponent(m_staticMeshComponent);
}

void AStaticMeshActor::Initialize()
{
	m_staticMeshComponent->Init();
}

shared_ptr<UStaticMeshComponent> AStaticMeshActor::GetStaticMeshComponent()
{
	return m_staticMeshComponent;
}
