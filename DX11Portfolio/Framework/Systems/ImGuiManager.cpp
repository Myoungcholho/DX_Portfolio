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
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(ImGuiDesc.Width, ImGuiDesc.Height);
	ImGui::StyleColorsLight();

	bool dxInit = ImGui_ImplDX11_Init(ImGuiDesc.Device.Get(), ImGuiDesc.DeviceContext.Get());
	bool winInit = ImGui_ImplWin32_Init(ImGuiDesc.Hwnd);
	assert(dxInit && winInit && "ImGui initialization failed");

	initialized = true;

	// nullptr 읽지 않도록 최초 한 프레임을 셋팅
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Render();

	m_drawDataBuffers[0] = CloneDrawData(ImGui::GetDrawData());
}

/// <summary>
/// 매 프레임에 시작
/// </summary>
void ImGuiManager::BeginFrame()
{
	if (!initialized) return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

/// <summary>
/// 매 마지막 프레임에 시작
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

	ImGui::Render();

	///////////////////////////////////////////////////////////////////////////

	{
		std::lock_guard<std::mutex> lock(m_drawDataMutex);

		FreeDrawData(m_drawDataBuffers[m_writeIndex]);
		m_drawDataBuffers[m_writeIndex] = CloneDrawData(ImGui::GetDrawData());

		// 다음 프레임에 쓸 버퍼로 전환
		m_writeIndex = (m_writeIndex + 1) % 3;
	}
}

void ImGuiManager::RenderDrawData(ID3D11DeviceContext* context)
{
	std::lock_guard<std::mutex> lock(m_drawDataMutex);

	// 최신 writeIndex 기준으로 렌더링할 index 결정
	int nextRenderIndex = (m_writeIndex + 2) % 3; // writeIndex 바로 전 프레임
	ImDrawData* drawData = m_drawDataBuffers[nextRenderIndex];

	if (drawData)
	{
		ImGui_ImplDX11_RenderDrawData(drawData);
		m_renderedIndex = nextRenderIndex;
	}
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
		ImDrawList* dstList = IM_NEW(ImDrawList)(srcList->_Data);  // _Data는 공유 가능

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