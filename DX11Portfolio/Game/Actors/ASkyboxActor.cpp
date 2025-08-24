#include "Pch.h"
#include "ASkyboxActor.h"

ASkyboxActor::ASkyboxActor()
{
	m_name = "SkyBox";
	m_skyboxComponent = make_shared<USkyboxComponent>();
	root = m_skyboxComponent;
	AddComponent(m_skyboxComponent);
}

void ASkyboxActor::Initialize()
{
	// 컴포넌트들 초기화
	m_skyboxComponent->Init();
}

shared_ptr<USkyboxComponent> ASkyboxActor::GetSkyboxComponent()
{
	return m_skyboxComponent;
}