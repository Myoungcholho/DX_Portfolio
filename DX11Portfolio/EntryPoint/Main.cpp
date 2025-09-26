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

	// Game Run
	UGameInstance* game = new UGameInstance();
	game->Init();

	UWorld* world = game->GetWorld();

	ComPtr<ID3D11Device> device = D3D::Get()->GetDeviceCom();
	ComPtr<ID3D11DeviceContext> context = D3D::Get()->GetDeviceContextCom();

	// SkyBox
	{
		ASkyboxActor* skyboxActor = world->SpawnActor<ASkyboxActor>();

		PBRMeshData skyboxMesh = GeometryGenerator::MakeBox(50.0f);
		std::reverse(skyboxMesh.indices.begin(), skyboxMesh.indices.end());

		skyboxActor->GetSkyboxComponent()->SetPBRMeshData(vector{ skyboxMesh });	
	}

	// 바닥
	{
		PBRMeshData ground = GeometryGenerator::MakeSquareGrid(2048, 2048, 20.f, { 40.0f,40.0f });
		string path = "../../../_Textures/PBR/rock-wall-mortar-ue/";
		ground.albedoTextureFilename = path + "rock-wall-mortar_albedo.png";
		ground.normalTextureFilename = path+ "rock-wall-mortar_normal-dx.png";
		ground.heightTextureFilename = path+ "rock-wall-mortar_height.png";
		ground.aoTextureFilename = path+ "rock-wall-mortar_ao.png";
		ground.metallicTextureFilename = path + "rock-wall-mortar_metallic.png";
		ground.roughnessTextureFilename = path + "rock-wall-mortar_roughness.png";

		AStaticMeshActor* staticActor = world->SpawnActor<AStaticMeshActor>(vector{ ground });

		//staticActor->GetStaticMeshComponent()->SetPBRMeshData(vector{ ground });
		staticActor->GetStaticMeshComponent()->SetRelativePosition(Vector3(0, -1, 0));
		staticActor->GetStaticMeshComponent()->SetRelativeRotation(Vector3(90.0f, 0.0f, 0.0f));  // 단위: 도 (Euler)
	}

	// 모델
	{
		APlayer* player = world->SpawnActor<APlayer>();

		vector<PBRMeshData> meshes = GeometryGenerator::ReadFromFile("stan_lee/", "scene.gltf");
		//vector<PBRMeshData> meshes = GeomtryGenerator::ReadFromFileModel("medieval_vagrant_knights/", "scene.gltf");

		player->GetStaticMeshComponent()->SetPBRMeshData(meshes);
		player->GetStaticMeshComponent()->SetRelativePosition(Vector3(0, -0.5f, -9.5f));
		player->GetStaticMeshComponent()->SetRelativeRotation(Vector3(0.0f, 0.0f, 0.0f));  // 단위: 도 (Euler)
	}

	// 조명
	{
		//ALight* light = world->SpawnActor<ALight>();

		//light->GetLightComponent()->SetRelativePosition(Vector3(0, 3, -14));
		//light->GetLightComponent()->SetRelativeRotation(Vector3(-25, 0, 0));
		//light->GetLightComponent()->SetLightType((uint32_t)LIGHT_DIRECTIONAL | LIGHT_SHADOW);
		//light->SetName("SpotLight");
	}

	// SkinnedMesh(1),(2)
	{
		

		string path = "Mixamo/Rumy/";
		/*vector<string> clipNames =
		{
			"CatwalkIdle.fbx", "CatwalkIdleToWalkForward.fbx",
			"CatwalkWalkForward.fbx", "CatwalkWalkForward03.fbx", "CatwalkWalkStop.fbx",
			"BreakdanceFreezeVar2.fbx"
		};*/
		vector<string> clipNames =
		{
			"Idle.fbx", "Walking60.fbx"
		};

		shared_ptr<AnimationData> aniData = make_shared<AnimationData>();

		// 캐릭터의 정점을 1회 읽음
		auto [meshes, _] = GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");

		for (auto& name : clipNames)
		{
			// 애니메이션 정보만 읽음
			auto [_, ani] = GeometryGenerator::ReadAnimationFromFile(path, name);

			// 골격같은 데이터는 여러번 초기화할 필요 없으므로 1회만
			// 바인드 포즈같은 데이터는 이후에 들어올 클립도 전부 같은 정보다.
			// 그래서 이후에는 추가만 해준다
			if (aniData->clips.empty()) {
				*aniData = move(ani);
			}
			else {
				// fbx에 클립이 하나만 있지 않다. 현재는 하나만 있지만.
				// 아무튼 맨 앞에 첫번째 클립만 사용하겠다는 것
				aniData->clips.push_back(ani.clips.front());
			}
		}

		ASkinnedTestActor* skinned[100];

		for (int i = 0; i < 2; ++i)
		{
			skinned[i] = world->SpawnActor<ASkinnedTestActor>();

			skinned[i]->GetSkeletalMeshComponent()->SetAssets(meshes, aniData);
			skinned[i]->GetSkeletalMeshComponent()->SetMaterialFactors(Vector3(1.0f), 0.8f, 0.0f);
			skinned[i]->GetSkeletalMeshComponent()->SetRelativePosition(Vector3(i, 0.0f, 0.0f));
			skinned[i]->GetSkeletalMeshComponent()->SetTrack(i, true, 1.0f);
		}

		/*skinned_02->GetSkeletalMeshComponent()->SetAssets(meshes, aniData);
		skinned_02->GetSkeletalMeshComponent()->SetMaterialFactors(Vector3(1.0f), 0.8f, 0.0f);
		skinned_02->GetSkeletalMeshComponent()->SetRelativePosition(Vector3(1.0f, 0.0f, 0.0f));
		skinned_02->GetSkeletalMeshComponent()->SetTrack(1, true, 1.0f);*/
	}

	// 모든 액터 배치가 끝났다면
	world->StartAllActors();

	CKeyboard::Create();
	CTimer::Create();
	CMouse::Create();
	CContext::Create();

	EditorApplication::SetWorld(game->GetWorld());
	EditorApplication::SetRenderer(game->GetRenderer());
	EditorApplication::Initialize();

	WPARAM result = Window::Run(game);

	delete game;

	EditorApplication::Release();

	CContext::Destroy();
	CMouse::Destroy();
	CTimer::Destroy();
	CKeyboard::Destroy();

	D3D::Destroy();
	Window::Destroy();

	DisableConsole();

	return (int)result;
}