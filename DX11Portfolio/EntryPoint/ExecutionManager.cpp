#include "Pch.h"
#include "ExecutionManager.h"

void ExecutionManager::CreateExecutables()
{
	cubeMapSky = new CubeMapSkyRenderer();

	executes.push_back(new BillboardSystem());
	
	// 물체
	//executes.push_back(new TestObjectCube());
	//executes.push_back(new TestObjectQuad());
	//executes.push_back(new TestObjectPlane()); // 수직 고양이
	//executes.push_back(new TestObjectGrid());
	//executes.push_back(new TestObjectCylinder());
	//executes.push_back(new TestObjectSphere());
	//executes.push_back(new TestObjectModel());
	// stan_lee / medieval_vagrant_knights
	//MeshGroup* obj = new MeshGroup("medieval_vagrant_knights/", "scene.gltf", "Player");
	//obj->GetTransform()->SetPosition(Vector3(0, -4.5, 14.5));
	//executes.push_back(obj);
	
	executes.push_back(new TextureGround());

	// Light
	LightCreate();

	// mirror
	/*Mirror* mirror1 = new Mirror("Mirror1");
	mirror1->GetTransform()->SetPosition(Vector3(3, -4.5, 13.5));
	mirror1->GetTransform()->SetRotation(Vector3(0, 90, 0));
	mirror1->GetTransform()->SetScale(Vector3(3, 3, 1));
	mirrors.push_back(mirror1);

	Mirror* mirror2 = new Mirror("Mirror2");
	mirror2->GetTransform()->SetPosition(Vector3(-3, -4.5, 13.5));
	mirror2->GetTransform()->SetRotation(Vector3(0, -90, 0));
	mirror2->GetTransform()->SetScale(Vector3(3, 3, 1));
	mirrors.push_back(mirror2);*/
}

void ExecutionManager::LightCreate()
{
	LightData pointLight;
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

	cubeMapSky->Initialize();

	for (IExecutable* execute : executes)
		execute->Initialize();

	for (Mirror* mirror : mirrors)
		mirror->Initialize();
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
	cubeMapSky->UpdateGUI();

	for (IExecutable* execute : executes)
		execute->UpdateGUI();

	for (Mirror* mirror : mirrors)
		mirror->UpdateGUI();
}

void ExecutionManager::Tick()
{
	cubeMapSky->Tick();

	for (IExecutable* execute : executes)
		execute->Tick();

	for (Mirror* mirror : mirrors)
		mirror->Tick();
}

void ExecutionManager::Render()
{
	// RTV , DSV 설정
	//D3D::Get()->StartRenderPass();

	// viewproj(b1)
	CContext::Get()->RenderConstantBuffer();

	// PipeLineState
	GraphicsDevice::Get()->ApplyDrawBasicDSS(0);
	//CContext::Get()->GetWireRender() ? GraphicsDevice::Get()->ApplyWireframeRasterizer() : GraphicsDevice::Get()->ApplySolidRasterizer();
	GraphicsDevice::Get()->ApplyBasicBlendState();

	// 0. 환경맵 렌더
	cubeMapSky->Render(false);

	// 1. 일반 물체를 렌더
	for (IExecutable* execute : executes)
		execute->Render();

	// 2. 거울 처리
	int mask = 1;
	// 원본 스텐실 값 초기화
	//D3D::Get()->ClearMainDepth(D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (Mirror* mirror : mirrors)
	{
		// DSV 복사및 OM바인딩
		D3D::Get()->SaveAndBindTempDepthStencil();

		// 거울 시작전 마스킹
		GraphicsDevice::Get()->ApplyMaskDSS(mask);
		// 거울 렌더로 마스킹
		mirror->Render();

		// 거울 위치에 반사된 물체들을 렌더링
		//D3D::Get()->ClearTempDepth(D3D11_CLEAR_DEPTH, 1.0f, 0);
		GraphicsDevice::Get()->ApplyDrawMaskedDSS(mask);
		//CContext::Get()->GetWireRender() ? GraphicsDevice::Get()->ApplyWireframeCCWRasterizer() : GraphicsDevice::Get()->ApplySolidCCWRasterizer();
		mirror->UpdateConstantBuffer();
		CContext::Get()->RenderMirrorConstantBuffer();

		cubeMapSky->Render(true);
		
		for (IExecutable* execute : executes)
			execute->Render();
		
		// 거울 그리기
		const float t = 1.0f - 0.2f;
		const float blendColor[] = { t,t,t,1.0f };
		GraphicsDevice::Get()->ApplyMirrorBlendState(blendColor);
		//CContext::Get()->GetWireRender() ? GraphicsDevice::Get()->ApplyWireframeRasterizer() : GraphicsDevice::Get()->ApplySolidRasterizer();
		CContext::Get()->RenderConstantBuffer();
		mirror->Render();
		
		// Blend 복원
		GraphicsDevice::Get()->ApplyBasicBlendState();
		++mask;
	}
}