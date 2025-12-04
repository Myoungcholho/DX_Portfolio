#include "Pch.h"
#include "ASkyboxActor.h"

ASkyboxActor::ASkyboxActor()
{
	SetName("SkyBox");

	/*skyboxComponent = make_shared<USkyboxComponent>();
	rootShared = skyboxComponent;
	AddComponent(skyboxComponent);*/

	skyboxComponent = CreateDefaultSubobject<USkyboxComponent>("SkyBoxComponent");
	SetRootComponent(skyboxComponent);
}

void ASkyboxActor::Initialize()
{
	// 컴포넌트들 초기화
	skyboxComponent->Init();
}