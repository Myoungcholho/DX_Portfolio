#include "Framework.h"
#include "ImGuiManager.h"

ImGuiManager * ImGuiManager::Instance = nullptr;
ImGUIDesc ImGuiManager::ImGuiDesc = {};
bool ImGuiManager::initialized = false;

#ifdef IMGUI_ENABLE_DOCKING
#pragma message("??IMGUI_ENABLE_DOCKING is enabled!")
#else
#pragma message("??IMGUI_ENABLE_DOCKING is NOT defined!")
#endif

ImGuiManager* ImGuiManager::Get()
{
	assert(Instance != nullptr);
	return Instance;
}

ImGuiManager::ImGuiManager()
{
	Initialize();
}

ImGuiManager::~ImGuiManager()
{
	for (auto& buffer : m_drawDataBuffers)
		FreeDrawData(buffer);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
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

void ImGuiManager::Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	io.DisplaySize = ImVec2(ImGuiDesc.Width, ImGuiDesc.Height);

	//ImGui::StyleColorsLight();
	ImGui::StyleColorsDark();

	bool winInit = ImGui_ImplWin32_Init(ImGuiDesc.Hwnd);
	bool dxInit = ImGui_ImplDX11_Init(ImGuiDesc.Device.Get(), ImGuiDesc.DeviceContext.Get());
	assert(dxInit && winInit && "ImGui initialization failed");

	initialized = true;

	// √π «¡∑π¿”ø° nullπÊ¡ˆ
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Render();

	m_drawDataBuffers[0] = CloneDrawData(ImGui::GetDrawData());
}

/// <summary>
/// Îß??ÑÎ†à?ÑÏóê ?úÏûë
/// </summary>
void ImGuiManager::BeginFrame()
{
	if (!initialized) return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ShowSimpleDockUI();
}

/// <summary>
/// Îß?ÎßàÏ?Îß??ÑÎ†à?ÑÏóê ?úÏûë
/// </summary>
void ImGuiManager::EndFrame()
{
	if (!initialized) return;

	// Render ImGui text contents
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::Begin("TextWindow", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus);

	for (auto& content : Contents)
	{
		ImVec2 pos(content.Position.x, content.Position.y);
		ImColor color(content.Color.R, content.Color.G, content.Color.B, content.Color.A);
		ImGui::GetWindowDrawList()->AddText(pos, color, content.Content.c_str());
	}
	Contents.clear();
	ImGui::End();

	//GizmoTest();

	ImGui::Render();

	//if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	//{
	//	ImGui::UpdatePlatformWindows(); // Ï∞??ùÏÑ± Î∞??ÅÌÉú Í∞±Ïã† (Î©îÏù∏ ?§Î†à???ÑÏàò)
	//}

	///////////////////////////////////////////////////////////////////////////

	{
		std::lock_guard<std::mutex> lock(m_drawDataMutex);

		FreeDrawData(m_drawDataBuffers[m_writeIndex]);
		m_drawDataBuffers[m_writeIndex] = CloneDrawData(ImGui::GetDrawData());

		// ?§Ïùå ?ÑÎ†à?ÑÏóê ??Î≤ÑÌçºÎ°??ÑÌôò
		m_writeIndex = (m_writeIndex + 1) % 3;
	}
}

/// <summary>
/// Render Thread?êÏÑúÎß??∏Ï∂ú?¥Ïïº?úÎã§.
/// </summary>
void ImGuiManager::RenderDrawData(ID3D11DeviceContext* context)
{
	std::lock_guard<std::mutex> lock(m_drawDataMutex);

	// ÏµúÏã† writeIndex Í∏∞Ï??ºÎ°ú ?åÎçîÎßÅÌï† index Í≤∞Ï†ï
	int nextRenderIndex = (m_writeIndex + 2) % 3; // writeIndex Î∞îÎ°ú ???ÑÎ†à??
	ImDrawData* drawData = m_drawDataBuffers[nextRenderIndex];

	if (drawData)
	{
		ImGui_ImplDX11_RenderDrawData(drawData);
		m_renderedIndex = nextRenderIndex;

		//// Î©Ä??Î∑∞Ìè¨?∏Í? ÏºúÏ†∏ ?àÎã§Î©??úÎ∏å Ï∞ΩÎì§ ?åÎçîÎßÅÎèÑ ?§Ìñâ
		//if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//{
		//	ImGui::RenderPlatformWindowsDefault(nullptr, context);
		//}
	}
}

void ImGuiManager::ShowSimpleDockUI()
{
	// 1. DockSpace ?ùÏÑ±
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin("DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar(2);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);
	ImGui::End();

	// 2. Í∞ÑÎã®???ÑÌÇπ Í∞Ä?•Ìïú Ï∞?
	ImGui::Begin("Hello Dock");
	ImGui::Text("?ÑÌÇπ ?åÏä§?∏Ïö© Ï∞ΩÏûÖ?àÎã§!");
	ImGui::End();
}

void ImGuiManager::GizmoTest()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetOrthographic(false/*!isPerspective*/);
	ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);


	ImGuizmo::BeginFrame();

	UINT width = ImGuiDesc.Width;
	UINT height = ImGuiDesc.Height;
	float windowWidth = (float)ImGui::GetWindowWidth();
	float windowHeight = (float)ImGui::GetWindowHeight();

	RECT rect = { 0, 0, 0, 0 };
	::GetClientRect(ImGuiDesc.Hwnd, &rect);

	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, width, height);

	Matrix viewMatirx;
	Matrix projectionMatirx;

	viewMatirx = CContext::Get()->GetViewMatrix();
	projectionMatirx = CContext::Get()->GetProjectionMatrix();

	Matrix modelMatrix;
	XMVECTOR translation = XMVectorSet(0.0f, -10.0f, 0.0f, 1.0f);
	XMVECTOR rotation = XMQuaternionIdentity();
	XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

	modelMatrix = XMMatrixScalingFromVector(scale) *
		XMMatrixRotationQuaternion(rotation) *
		XMMatrixTranslationFromVector(translation);

	ImGuizmo::Manipulate(*viewMatirx.m, *projectionMatirx.m,
		ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, *modelMatrix.m);
}

ImDrawData* ImGuiManager::CloneDrawData(ImDrawData* src)
{
	if (!src)
		return nullptr;

	ImDrawData* clone = IM_NEW(ImDrawData)(*src);
	clone->CmdLists.resize(src->CmdListsCount);

	for (int i = 0; i < src->CmdListsCount; ++i)
	{
		const ImDrawList* srcList = src->CmdLists[i];
		ImDrawList* dstList = IM_NEW(ImDrawList)(srcList->_Data);  // _Data??Í≥µÏú† Í∞Ä??

		// CmdBuffer deep copy
		dstList->CmdBuffer.resize(srcList->CmdBuffer.Size);
		memcpy(dstList->CmdBuffer.Data, srcList->CmdBuffer.Data, sizeof(ImDrawCmd) * srcList->CmdBuffer.Size);

		// IdxBuffer deep copy
		dstList->IdxBuffer.resize(srcList->IdxBuffer.Size);
		memcpy(dstList->IdxBuffer.Data, srcList->IdxBuffer.Data, sizeof(ImDrawIdx) * srcList->IdxBuffer.Size);

		// VtxBuffer deep copy
		dstList->VtxBuffer.resize(srcList->VtxBuffer.Size);
		memcpy(dstList->VtxBuffer.Data, srcList->VtxBuffer.Data, sizeof(ImDrawVert) * srcList->VtxBuffer.Size);

		clone->CmdLists[i] = dstList;
	}

	return clone;
}

void ImGuiManager::FreeDrawData(ImDrawData* drawData)
{
	if (!drawData) return;

	for (int i = 0; i < drawData->CmdLists.Size; ++i)
	{
		IM_DELETE(drawData->CmdLists[i]);
	}
	drawData->CmdLists.clear();

	IM_DELETE(drawData);
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