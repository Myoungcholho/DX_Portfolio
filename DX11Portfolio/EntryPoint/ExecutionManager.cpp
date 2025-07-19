#include "Pch.h"
#include "ExecutionManager.h"

void ExecutionManager::CreateExecutables()
{
	executes.push_back(new BillboardSystem());
	executes.push_back(new CubeMapSkyRenderer());
	
	// 물체
	executes.push_back(new TestObjectCube());
	executes.push_back(new TestObjectQuad());
	executes.push_back(new TestObjectPlane()); // 수직 고양이
	executes.push_back(new TestObjectGrid());
	//executes.push_back(new TestObjectCylinder());
	//executes.push_back(new TestObjectSphere());
	//executes.push_back(new TestObjectModel());
	// stan_lee / medieval_vagrant_knights
	executes.push_back(new MeshGroup("stan_lee/", "scene.gltf"));

	//executes.push_back(new TextureGround());

	// Light
	LightCreate();
}

void ExecutionManager::LightCreate()
{
	Light pointLight;
	pointLight.position = Vector3(1.0f, -2.5f, 10.0f);
	pointLight.radiance = Vector3(10.0f);
	pointLight.fallOffStart = 0.0;
	pointLight.spotPower = 10.0f;
	pointLight.fallOffEnd = 10.0f;
	pointLight.id = 1;
	pointLight.type = int(LightType::Point);

	Engine::Get()->GetLightManager()->AddLight(pointLight);
}

void ExecutionManager::Initialize()
{
	CreateExecutables();

	for (IExecutable* execute : executes)
		execute->Initialize();
}

void ExecutionManager::Destroy()
{
	for (IExecutable* execute : executes)
	{
		execute->Destroy();

		if (execute != nullptr)
		{
			delete execute;
			execute = nullptr;
		}
	}
}

void ExecutionManager::UpdateGUI()
{
	for (IExecutable* execute : executes)
		execute->UpdateGUI();
}

void ExecutionManager::Tick()
{
	for (IExecutable* execute : executes)
		execute->Tick();
}

void ExecutionManager::Render()
{
	for (IExecutable* execute : executes)
		execute->Render();
}