#include "Pch.h"
#include "ASkinnedTestActor.h"

ASkinnedTestActor::ASkinnedTestActor()
{
	m_name = "SkinnedTest";
	m_skeletalMeshComponent = make_shared<USkeletalMeshComponent>();
	root = m_skeletalMeshComponent;
	AddComponent(m_skeletalMeshComponent);
}

void ASkinnedTestActor::Initialize()
{
	m_skeletalMeshComponent->Init();

}

shared_ptr<USkeletalMeshComponent> ASkinnedTestActor::GetSkeletalMeshComponent()
{
	return m_skeletalMeshComponent;
}
