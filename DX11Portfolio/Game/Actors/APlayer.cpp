#include "Pch.h"
#include "APlayer.h"

APlayer::APlayer()
{
	m_staticMeshComponent = make_shared<UStaticMeshComponent>();
	root = m_staticMeshComponent;
	AddComponent(m_staticMeshComponent);
}

void APlayer::Initialize()
{
	m_staticMeshComponent->Init();
}

shared_ptr<UStaticMeshComponent> APlayer::GetStaticMeshComponent()
{
	return m_staticMeshComponent;
}
