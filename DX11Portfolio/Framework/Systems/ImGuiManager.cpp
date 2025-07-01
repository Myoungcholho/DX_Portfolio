#include "Framework.h"
#include "ImGuiManager.h"

ImGuiManager* ImGuiManager::Instance = nullptr;
ImGUIDesc ImGuiManager::ImGuiDesc = {};
bool ImGuiManager::initialized = false;

ImGuiManager* ImGuiManager::Get()
{
	assert(Instance != nullptr);
	return Instance;
}

void ImGuiManager::Create()
{
	assert(Instance == nullptr);
	Instance = new ImGuiManager();
}

void ImGuiManager::Destroy()
{
	if (Instance != nullptr)
	{
		delete Instance;
		Instance = nullptr;
	}
}

void ImGuiManager::SetDesc(const ImGUIDesc& InDesc)
{
	ImGuiDesc = InDesc;
}

void ImGuiManager::Shutdown()
{

}

void ImGuiManager::BeginFrame()
{
	if (!initialized) return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::EndFrame()
{
	if (!initialized) return;
	
	
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::Render()
{
	// TODO : Contents 요소들을 출력
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowBgAlpha(0.0f);


	ImGui::Begin
	(
		"TextWindow", NULL,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus
	);


	for (FGuiText content : Contents)
	{
		ImVec2 position = ImVec2(content.Position.x, content.Position.y);
		ImColor color = ImColor(content.Color.R, content.Color.G, content.Color.B, content.Color.A);

		ImGui::GetWindowDrawList()->AddText(position, color, content.Content.c_str());
	}
	Contents.clear();


	ImGui::End();
}

void ImGuiManager::RenderText(float x, float y, float r, float g, float b, string content)
{
	RenderText(x, y, r, g, b, 1.0f, content);
}

void ImGuiManager::RenderText(float x, float y, float r, float g, float b, float a, string content)
{
	FGuiText text;
	text.Position = Vector2(x, y);
	text.Color = FColor(r, g, b, a);
	text.Content = content;

	Contents.push_back(text);
}

void ImGuiManager::Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	io.DisplaySize = ImVec2(ImGuiDesc.Width, ImGuiDesc.Height);
	ImGui::StyleColorsLight();

	// Backend 연결
	bool dxInit = ImGui_ImplDX11_Init(ImGuiDesc.Device.Get(), ImGuiDesc.DeviceContext.Get());
	bool winInit = ImGui_ImplWin32_Init(ImGuiDesc.Hwnd);
	assert(dxInit && winInit && "ImGui 초기화 실패");

	initialized = true;
}

ImGuiManager::ImGuiManager()
{
	Initialize();
}

ImGuiManager::~ImGuiManager()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}
