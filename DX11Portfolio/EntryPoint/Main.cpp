#include "Pch.h"
#include "Main.h"
#include "Systems/Window.h"
#include <memory>
#include "ExecutionManager.h"

int WINAPI WinMain(HINSTANCE InInstance, HINSTANCE InPrevInstance, LPSTR InParam, int command)
{
	EnableConsole();

	WindowDesc winDesc;
	winDesc.AppName = L"DirectXPortfolio";
	winDesc.Instance = InInstance;
	winDesc.Width = 1920; 
	winDesc.Height = 1080;
	winDesc.Background = Color(0.2f, 0.2f, 0.2f, 1.0f);

	// 윈도우 생성
	HWND hwnd = Window::Create(winDesc);

	// D3D 초기화
	D3DDesc d3dDesc = { hwnd, winDesc.Width, winDesc.Height, true };
	D3D::SetDesc(d3dDesc);
	D3D::Create();
	D3D::Get()->GraphicsInit();

	// ImGUI 초기화
	/*ImGUIDesc imguiDesc = {};
	imguiDesc.Device = D3D::Get()->GetDeviceCom();
	imguiDesc.DeviceContext = D3D::Get()->GetDeviceContextCom();
	imguiDesc.Width = winDesc.Width;
	imguiDesc.Height = winDesc.Height;
	imguiDesc.Hwnd = hwnd;
	ImGuiManager::SetDesc(imguiDesc);
	ImGuiManager::Create();*/


	// Game Run
	UGameInstance* game = new UGameInstance();
	game->Init();

	UWorld* world = game->GetWorld();

	ComPtr<ID3D11Device> device = D3D::Get()->GetDeviceCom();
	ComPtr<ID3D11DeviceContext> context = D3D::Get()->GetDeviceContextCom();

	// SkyBox
	{
		ASkyboxActor* skyboxActor = world->SpawnActor<ASkyboxActor>();

		PBRMeshData skyboxMesh = GeomtryGenerator::MakeBox(50.0f);
		std::reverse(skyboxMesh.indices.begin(), skyboxMesh.indices.end());

		skyboxActor->GetSkyboxComponent()->SetPBRMeshData(vector{ skyboxMesh });	
	}

	// 바닥
	{
		AStaticMeshActor* staticActor = world->SpawnActor<AStaticMeshActor>();

		PBRMeshData ground = GeomtryGenerator::MakeSquareGrid(2048, 2048, 20.f, { 40.0f,40.0f });
		string path = "../../../_Textures/PBR/rock-wall-mortar-ue/";
		ground.albedoTextureFilename = path + "rock-wall-mortar_albedo.png";
		ground.normalTextureFilename = path+ "rock-wall-mortar_normal-dx.png";
		ground.heightTextureFilename = path+ "rock-wall-mortar_height.png";
		ground.aoTextureFilename = path+ "rock-wall-mortar_ao.png";
		ground.metallicTextureFilename = path + "rock-wall-mortar_metallic.png";
		ground.roughnessTextureFilename = path + "rock-wall-mortar_roughness.png";

		staticActor->GetStaticMeshComponent()->SetPBRMeshData(vector{ ground });
		staticActor->GetStaticMeshComponent()->SetRelativePosition(Vector3(0, -1, 0));
		staticActor->GetStaticMeshComponent()->SetRelativeRotation(Vector3(90.0f, 0.0f, 0.0f));  // 단위: 도 (Euler)
	}

	// 플레이어
	{
		APlayer* player = world->SpawnActor<APlayer>();

		//vector<PBRMeshData> meshes = GeomtryGenerator::ReadFromFileModel("medieval_vagrant_knights/", "scene.gltf");
		vector<PBRMeshData> meshes = GeomtryGenerator::ReadFromFileModel("stan_lee/", "scene.gltf");

		player->GetStaticMeshComponent()->SetPBRMeshData(meshes);
		player->GetStaticMeshComponent()->SetRelativePosition(Vector3(0, -0.5f, -9.5f));
		player->GetStaticMeshComponent()->SetRelativeRotation(Vector3(0.0f, 0.0f, 0.0f));  // 단위: 도 (Euler)
	}

	// 조명
	{
		ALight* light = world->SpawnActor<ALight>();

		light->GetLightComponent()->SetRelativePosition(Vector3(0, 3, -14));
		light->GetLightComponent()->SetRelativeRotation(Vector3(-25, 0, 0));
		light->GetLightComponent()->SetLightType((uint32_t)LIGHT_DIRECTIONAL | LIGHT_SHADOW);
		light->SetName("SpotLight");
	}

	{
		//ALight* light = world->SpawnActor<ALight>();

		//light->GetLightComponent()->SetRelativePosition(Vector3(-10, 10, -10));
		//light->GetLightComponent()->SetRelativeRotation(Vector3(-90, -90, 0));
		//light->GetLightComponent()->SetLightType((uint32_t)LIGHT_DIRECTIONAL | LIGHT_SHADOW);
		//light->SetName("DirectionLight");
	}

	// 모든 액터 배치가 끝났다면
	world->StartAllActors();

	WPARAM result = Window::Run(game);

	delete game;

	//ImGuiManager::Destroy();
	D3D::Destroy();
	Window::Destroy();

	//int a = 50;
	//cout << a;

	return (int)result;
}