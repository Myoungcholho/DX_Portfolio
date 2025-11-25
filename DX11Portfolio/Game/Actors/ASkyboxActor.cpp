#include "Pch.h"
#include "ASkyboxActor.h"

ASkyboxActor::ASkyboxActor()
{
	name = "SkyBox";
	skyboxComponent = make_shared<USkyboxComponent>();
	root = skyboxComponent;
	AddComponent(skyboxComponent);
}

void ASkyboxActor::Initialize()
{
	// 컴포넌트들 초기화
	skyboxComponent->Init();
}

shared_ptr<USkyboxComponent> ASkyboxActor::GetSkyboxComponent()
{
	return skyboxComponent;
}