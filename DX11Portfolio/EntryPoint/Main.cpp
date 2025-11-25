#include "Pch.h"
#include "Main.h"
#include "Systems/Window.h"
#include <memory>
#include "ExecutionManager.h"

#pragma comment(lib, "Game.lib")
#pragma comment(linker, "/INCLUDE:AMyController_Anchor")

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
		reverse(skyboxMesh.indices.begin(), skyboxMesh.indices.end());

		shared_ptr<const CPUMeshAsset> asset = CPUAssetManager::CreateProcedural("Procedural:Skybox:Box50", vector{ skyboxMesh });
		
		skyboxActor->GetSkyboxComponent()->SetPBRMeshData(asset);
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

		shared_ptr<const CPUMeshAsset> asset = CPUAssetManager::CreateProcedural("Procedural:Ground:2048", vector{ ground });

		AStaticMeshActor* staticActor = world->SpawnActor<AStaticMeshActor>(asset);

		//staticActor->GetStaticMeshComponent()->SetPBRMeshData(vector{ ground });
		staticActor->GetStaticMeshComponent()->SetRelativePosition(Vector3(0, -1, 0));
		staticActor->GetStaticMeshComponent()->SetRelativeRotation(Vector3(90.0f, 0.0f, 0.0f));  // 단위: 도 (Euler)
	}

	// 모델
	{

		shared_ptr<const CPUMeshAsset> cpu1 = CPUAssetManager::LoadCPUMesh("stan_lee/", "scene.gltf");

		//APlayer* player = world->SpawnActor<APlayer>();
		//player->GetStaticMeshComponent()->SetPBRMeshData(cpu1);
		//player->GetStaticMeshComponent()->SetRelativePosition(Vector3(0, -0.5f, -9.5f));
		//player->GetStaticMeshComponent()->SetRelativeRotation(Vector3(0.0f, 0.0f, 0.0f));  // 단위: 도 (Euler)

		APlayer* play[100];

		for (int i = 0; i < 0; ++i)
		{
			for (int j = 0; j < 0; ++j)
			{
				play[i] = world->SpawnActor<APlayer>();

				play[i]->GetStaticMeshComponent()->SetPBRMeshData(cpu1);
				play[i]->GetStaticMeshComponent()->SetRelativePosition(Vector3(i, -0.5f, j));
				play[i]->GetStaticMeshComponent()->SetRelativeRotation(Vector3(0.0f, 0.0f, 0.0f));  // 단위: 도 (Euler)
			}
		}
	}

	// 조명
	{
		ALight* light = world->SpawnActor<ALight>();

		light->GetLightComponent()->SetLightType((uint32_t)LIGHT_DIRECTIONAL);
		light->GetLightComponent()->SetRelativePosition(Vector3(0, 20, -14));
		light->GetLightComponent()->SetRelativeRotation(Vector3(-25, 0, 0));
		light->SetName("DirectionLight");
	}

	// 모델 로드 테스트
	{
		string path = "Mixamo/PaladinJNordstrom/";

		vector<string> clipNames =
		{
			//"MeleeAttack.fbx",
			//"ComboAttack.fbx",
			"Idle.fbx",
			//"EquipUnderarm.fbx",
			//"MeleeKick.fbx",
			//"RunForward.fbx",
		};

		shared_ptr<AnimationData> aniData = make_shared<AnimationData>();

		auto [meshes, _] = GeometryGenerator::ReadAnimationFromFile(path, "PaladinJNordstrom.fbx");

		shared_ptr<const CPUMeshAsset> asset = CPUAssetManager::CreateProcedural("PaladinJNordstrom:character", meshes);

		for (auto& name : clipNames)
		{
			auto [_, ani] = GeometryGenerator::ReadAnimationFromFile(path, name);

			if (aniData->clips.empty()) {
				*aniData = move(ani);
			}
			else {
				aniData->clips.push_back(ani.clips.front());
			}
		}
		CPUAssetManager::SaveAnimation("PaladinJNordstrom:Animation", aniData);

		ASkinnedTestActor* skinned = world->SpawnActor<ASkinnedTestActor>();
		skinned->GetSkeletalMeshComponent()->SetAssets(asset, aniData);
		skinned->GetSkeletalMeshComponent()->SetMaterialFactors(Vector3(1.0f), 0.8f, 0.0f);

		skinned->GetSkeletalMeshComponent()->SetRelativePosition(Vector3(1.0f, -0.5f, -9.f));
		skinned->GetSkeletalMeshComponent()->SetTrack(0, true, 1.0f);
		skinned->SetName("Paladin");
	}

	// SkinnedMesh(1),(2)
	{
		string path = "Mixamo/Rumy/";

		vector<string> clipNames =
		{
			"Idle.fbx", 
			//"Walking60.fbx",
			//"backward60.fbx",
			//"left60.fbx", "right60.fbx", "Run60.fbx",
			//"RootWalking.fbx",
			//"CoverToStand.fbx"
			//"Walking60_InPlace.fbx",
			//"Walking60_Not_InPlace.fbx"
		};

		shared_ptr<AnimationData> aniData = make_shared<AnimationData>();

		// 캐릭터의 정점을 1회 읽음
		auto [meshes, _] = GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");

		shared_ptr<const CPUMeshAsset> asset = CPUAssetManager::CreateProcedural("Rumy:character", meshes);

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
		CPUAssetManager::SaveAnimation("Rumy:Animation", aniData);


		ASkinnedTestActor* skinned[400];

		/*skinned[0] = world->SpawnActor<ASkinnedTestActor>();

		skinned[0]->GetSkeletalMeshComponent()->SetAssets(asset, aniData);
		skinned[0]->GetSkeletalMeshComponent()->SetMaterialFactors(Vector3(1.0f), 0.8f, 0.0f);
		skinned[0]->GetSkeletalMeshComponent()->SetRelativePosition(Vector3(0, -0.5f, -9.f));
		skinned[0]->GetSkeletalMeshComponent()->SetTrack(0, true, 1.0f);*/

		for (int i = 0; i < 0; ++i)
		{
			for (int j = 0; j < 0; ++j)
			{
				int index = i * 10 + j;

				skinned[index] = world->SpawnActor<ASkinnedTestActor>();

				skinned[index]->GetSkeletalMeshComponent()->SetAssets(asset, aniData);
				skinned[index]->GetSkeletalMeshComponent()->SetMaterialFactors(Vector3(1.0f), 0.8f, 0.0f);
				skinned[index]->GetSkeletalMeshComponent()->SetRelativePosition(Vector3(-15.f + i*2, -0.5f, -15.f + j*2));
				skinned[index]->GetSkeletalMeshComponent()->SetTrack(i % clipNames.size(), true, 1.0f);

				string name = "skinned" + to_string(index);
				skinned[index]->SetName(name);
			}
		}


		// Pawn과 Controller 테스트
		{
			/*APawnTest* pawnActor = static_cast<APawnTest*>(ClassID::Create("APawnTest", world));

			pawnActor->GetSkeletalMeshComponent()->SetAssets(asset, aniData);
			pawnActor->GetSkeletalMeshComponent()->SetMaterialFactors(Vector3(1.0f), 0.8f, 0.0f);
			pawnActor->GetSkeletalMeshComponent()->SetRelativePosition(Vector3(0, 0.f, -9.f));
			pawnActor->GetSkeletalMeshComponent()->SetTrack(0, true, 1.0f);

			APlayerController* playerController = world->SpawnActor<APlayerController>();
			AMyController* playerController = static_cast<AMyController*>(ClassID::Create("AMyController", world));
			playerController->Possess(pawnActor);

			AMyController* p = new AMyController();*/
		}

		// GameMode 테스트
		{
			//APawnTest* p = new APawnTest();		// [테스트]강제 참조, 심볼 추가로 .obj링크용

			//AGameMode* gameMode = static_cast<AGameMode*>(ClassID::Create("AGameMode", world));

			//// --- 기본 Pawn / Controller 클래스 이름 지정 ---
			//gameMode->SetDefaultPawnClass("APawnTest");
			//gameMode->SetDefaultControllerClass("AMyController");

			//// --- 게임 시작 ---
			//gameMode->StartPlay();


			//// Controller Pawn 변경 테스트
			//APawnTest* pawnActor = static_cast<APawnTest*>(ClassID::Create("APawnTest", world));
			//pawnActor->GetSkeletalMeshComponent()->SetRelativePosition(Vector3(0, -0.5f, -8.f));
			//pawnActor->SetName("Pawn01");
			//
			//APawnTest* pawnActor1 = static_cast<APawnTest*>(ClassID::Create("APawnTest", world));
			//pawnActor1->GetSkeletalMeshComponent()->SetRelativePosition(Vector3(0, -0.5f, -7.f));
			//pawnActor1->SetName("Pawn02");

			//APawnTest* pawnActor2 = static_cast<APawnTest*>(ClassID::Create("APawnTest", world));
			//pawnActor2->GetSkeletalMeshComponent()->SetRelativePosition(Vector3(0, -0.5f, -6.f));
			//pawnActor2->SetName("Pawn03");
		}
	}

	// 모든 액터 배치가 끝났다면
	world->StartAllActors();

	CKeyboard::Create();
	CTimer::Create();
	CMouse::Create();
	CContext::Create();
	PerfMon::Init(D3D::Get()->GetDevice(), 3);

	EditorApplication::SetWorld(game->GetWorld());
	EditorApplication::SetRenderer(game->GetRenderer());
	EditorApplication::Initialize();

	// FPS확인용 데이터 로딩 시간은 제외하기 위해 시간시간 재셋팅
	g_profile.SetStartTime();		

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