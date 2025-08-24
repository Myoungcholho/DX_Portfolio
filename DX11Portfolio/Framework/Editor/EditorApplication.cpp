#include "Framework.h"
#include "EditorApplication.h"

ImGuiWindowFlags EditorApplication::mFlag = ImGuiWindowFlags_None;
ImGuiDockNodeFlags EditorApplication::mDockspaceFlags = ImGuiDockNodeFlags_None;
EditorApplication::eState EditorApplication::mState = EditorApplication::eState::Active;
bool EditorApplication::mFullScreen = true;
bool EditorApplication::mPadding = false;
std::map<std::wstring, EditorWindow*> EditorApplication::mEditorWindows;
Vector2 EditorApplication::mViewportBounds[2] = {};
Vector2 EditorApplication::mViewportSize;
bool EditorApplication::mViewportFocused = false;
bool EditorApplication::mViewportHovered = false;
UWorld* EditorApplication::m_world = nullptr;
URenderer* EditorApplication::m_renderer = nullptr;

bool EditorApplication::Initialize()
{
	imGguiInitialize();

	InspectorWindow* inspector = new InspectorWindow();
	mEditorWindows.insert(std::make_pair(L"InspectorWindow", inspector));
	inspector->Initialize();

	WorldOutlinerWindow* outliner = new WorldOutlinerWindow();
	outliner->SetWorld(m_world);
	mEditorWindows.insert(std::make_pair(L"Outliner", outliner));

	ComponentHierarchyWindow* componentWindow = new ComponentHierarchyWindow();
	mEditorWindows.insert(std::make_pair(L"Seleted Actor", componentWindow));

	RendererSettingsWindow* rendererWindow = new RendererSettingsWindow();
	rendererWindow->SetRenderer(m_renderer);
	mEditorWindows.insert(std::make_pair(L"RendererWindow", rendererWindow));

	return true;
}

void EditorApplication::Run()
{
	Update();
	OnGUI();
}

void EditorApplication::Update()
{
}

void EditorApplication::OnGUI()
{
	imGuiRender();
}


void EditorApplication::Release()
{
	for (auto iter : mEditorWindows)
	{
		delete iter.second;
		iter.second = nullptr;
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void EditorApplication::OpenProject()
{
}

void EditorApplication::NewScene()
{
}

void EditorApplication::SaveScene()
{
}

void EditorApplication::SaveSceneAs()
{
}

void EditorApplication::OpenScene(const std::filesystem::path& path)
{
}

//private//////////////////////////////////////////////////////////////////////

bool EditorApplication::imGguiInitialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(D3D::Get()->GetDesc().Handle);
	ImGui_ImplDX11_Init(D3D::Get()->GetDevice(), D3D::Get()->GetDeviceContext());	

	return false;
}

void EditorApplication::imGuiRender()
{
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();

	//////////////////////////////////////////////////////////////////////////////


	mFlag = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;				// 메뉴바 표시 + 다른 창 도킹 불가능
	if (mFullScreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();						// 메인 뷰포트 창 정보 가져오기
		ImGui::SetNextWindowPos(viewport->Pos);									// DockSpace창 메인 뷰포트 좌표에 맞춤
		ImGui::SetNextWindowSize(viewport->Size);								// DockSpace창 크기 맞춤
		ImGui::SetNextWindowViewport(viewport->ID);								// 창이 속할 뷰포트 ID설정, (멀티 뷰포트에서 필요)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);				// 창 모서리 둥글게하지 않음
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);				// 창 테두리 두께 0
		mFlag |= ImGuiWindowFlags_NoTitleBar									// 제목 표시줄 X
			| ImGuiWindowFlags_NoCollapse										// 접기 X
			| ImGuiWindowFlags_NoResize											// 리사이즈 X
			| ImGuiWindowFlags_NoMove;											// 이동 기능 비활성화

		mFlag |= ImGuiWindowFlags_NoBringToFrontOnFocus							// 포커스 이동 방지
			| ImGuiWindowFlags_NoNavFocus;										// 다른 창 위로 가져오기 방지
	}

	// 중앙 노드 패스스루 모드일 경우
	// - DockSpace 중앙 영역이 투명해지고, 하위 창이 없는 경우 배경이 비어 보임
	// - 배경을 그리지 않도록 플래그 추가
	if (mDockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		mFlag |= ImGuiWindowFlags_NoBackground;

	// 창 내부 패딩을 0으로 설정 (DockSpace 창 안쪽 여백 제거)
	// - DockSpace를 창 전체에 딱 맞게 채우기 위해 필요
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	bool Active = static_cast<bool>(mState);
	ImGui::Begin("EditorApplication", &Active, mFlag);							// DockSpace는 ImGuiBegin위에 호출되어야하므로 열기
	ImGui::PopStyleVar();

	if (mFullScreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;									// 현재 창의 최소 가로 크기 값 백업
	style.WindowMinSize.x = 370.0f;												// DockSpace창이 너무 좁게 줄어드는 것 방지로 임시 설정
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)						// Docking 기능이 활성화되어 있다면
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");						// DockSpace의 고유 ID 생성
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), mDockspaceFlags);	// (DockSpace 식별자, DockSpace 초기 크기(0,0)이라면 전부, DockSpace 동작 플래그
	}

	style.WindowMinSize.x = minWinSizeX;										// 창 최소 가로 크기 원래 값 복원

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
				OpenProject();

			ImGui::Separator();

			if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				NewScene();

			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				SaveScene();

			if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
				SaveSceneAs();

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
				return;

			ImGui::EndMenu();
		}

		//if (ImGui::BeginMenu("Script"))
		//{
		//	if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
		//	{
		//		//ScriptEngine::ReloadAssembly(); 추후 C#스크립트 추가기능이 생기면 추가할 예정
		//	}

		//	ImGui::EndMenu();
		//}

		ImGui::EndMenuBar();
	}

	for (auto iter : mEditorWindows)
		iter.second->Run();


	// viewport
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });				// 창 안쪽 여백 제거(씬 이미지를 창 전체에 맞추기 위함
	ImGui::Begin("Scene");															// Scene창 시작

	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();					// 창 내부의 좌상단 상대 좌표
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();					// 창 내부의 우하단 상대 좌표
	auto viewportOffset = ImGui::GetWindowPos();									// 현재 창의 화면상 절대 좌표

	// 좌상단 절대 좌표 계산 후 저장
	const int letTop = 0;
	mViewportBounds[letTop] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };

	// 우하단 절대 좌표 계산 후 저장
	const int rightBottom = 1;
	mViewportBounds[rightBottom] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	// ---------- 입력 포커스/호버 상태 ----------
	mViewportFocused = ImGui::IsWindowFocused();									// Scene 창이 포커스를 가지고 있는지?
	mViewportHovered = ImGui::IsWindowHovered();									// 마우스가 Scene창 위에 있는지?

	// (TODO) 마우스/키보드 입력 처리 (카메라 이동, 오브젝트 선택 등)

	// ---------- Scene 뷰 크기 및 이미지 렌더링 ----------
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();						// Scene 창 내부의 가용 공간 크기
	mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };					// 뷰포트 사이즈 갱신

	ImGui::Image(
		(ImTextureID)D3D::Get()->GetFinalLDR_SRV().Get(),							// SRV를 ImTextureID로 캐스팅
		ImVec2{ mViewportSize.x, mViewportSize.y },									// 이미지 크기
		ImVec2{ 0, 0 },																// UV 좌상단
		ImVec2{ 1, 1 }																// UV 우하단
	);

	// ---------- Gizmo ----------
	EditorGizmoSystem::Draw(CContext::Get()->GetViewMatrix(), CContext::Get()->GetProjectionMatrix(),
		ImVec2(mViewportBounds[0].x, mViewportBounds[0].y),  // 윈도우 컨텐츠의 절대 좌상단
		ImVec2(mViewportSize.x, mViewportSize.y));           // Scene 뷰포트 크기


	// ---------- 드래그&드롭 : Scene 열기 ----------
	//if (ImGui::BeginDragDropTarget())												// Scene 창이 드랍 타겟이 될 수 있는 상태인지 체크
	//{
	//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROJECT_ITEM"))// "PROJECT_ITEM" 타입의 드랍 페이로드를 수락
	//	{
	//		const wchar_t* path = (const wchar_t*)payload->Data;					// 페이로드에 포함된 경로(유니코드 문자열) 가져오기
	//		OpenScene(path);														// 해당 경로의 Scene 파일 열기
	//	}
	//	ImGui::EndDragDropTarget();
	//}

	


	ImGui::End();																	// Scene
	ImGui::PopStyleVar();

	ImGui::End();																	// dockspace end

	//////////////////////////////////////////////////////////////////////////////

	ImGui::Render();

	ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();
	ID3D11RenderTargetView* rtv = D3D::Get()->GetBackBufferRTV().Get();
	context->OMSetRenderTargets(1, &rtv, nullptr);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}