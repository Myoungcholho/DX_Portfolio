#include "Framework.h"
#include "RendererSettingsWindow.h"
#include "Renders/URenderer.h"

RendererSettingsWindow::RendererSettingsWindow()
{
	SetName("Renderer Settings");
	SetSize(ImVec2(300, 600));
}

RendererSettingsWindow::~RendererSettingsWindow()
{
}

void RendererSettingsWindow::Initialize() {}

void RendererSettingsWindow::Run()
{
	bool active = (GetState() == eState::Active);
	ImGui::Begin(GetName().c_str(), &active, GetFlag());

	Update();
	OnGUI();

	ImGui::End();
}
void RendererSettingsWindow::Update() {}

void RendererSettingsWindow::OnGUI()
{
    bool anyChanged = false;

    // ---- Exposure / Gamma ----
    ImGui::SeparatorText("Tone Mapping");
    {
        float exposure = m_renderer->GetExposure();       // 0.0 ~ 10.0
        float gamma = m_renderer->GetGamma();          // 0.1 ~ 5.0 (보통 2.2)

        if (ImGui::DragFloat("Exposure", &exposure, 0.01f, 0.0f, 10.0f))
        {
            m_renderer->SetExposure(exposure);
            anyChanged = true;
        }
        if (ImGui::DragFloat("Gamma", &gamma, 0.01f, 0.1f, 5.0f))
        {
            m_renderer->SetGamma(gamma);
            anyChanged = true;
        }
    }

    // ---- Bloom ----
    ImGui::SeparatorText("Bloom");
    {
        float bloom = m_renderer->GetBloomStrength();     // 0.0 ~ 1.0 (원하는 범위로)
        if (ImGui::DragFloat("Bloom Strength", &bloom, 0.01f, 0.0f, 1.0f))
        {
            m_renderer->SetBloomStrength(bloom);
            anyChanged = true;
        }
        int type = m_renderer->GetFilter();
        const char* items[] = { "LinearToneMapping (1)", "FilmicToneMapping (2)" ,"Uncharted2ToneMapping (3)", "lumaBasedReinhardToneMapping (4)"};
        if (ImGui::Combo("filter Mode", &type, items, IM_ARRAYSIZE(items)))
        {
            m_renderer->SetFilter(type);
            anyChanged = true;
        }
    }

    // ---- Fog / Depth ----
    ImGui::SeparatorText("Fog / Depth");
    {
        float depthScale = m_renderer->GetDepthScale();  // 0.0 ~ 2.0 등
        float fogStrength = m_renderer->GetFogStrength(); // 0.0 ~ 1.0

        if (ImGui::DragFloat("Depth Scale", &depthScale, 0.01f, 0.0f, 2.0f))
        {
            m_renderer->SetDepthScale(depthScale);
            anyChanged = true;
        }
        if (ImGui::DragFloat("Fog Strength", &fogStrength, 0.01f, 0.0f, 1.0f))
        {
            m_renderer->SetFogStrength(fogStrength);
            anyChanged = true;
        }
    }

    // ---- Mode (PostEffectsConstants.mode) ----
    ImGui::SeparatorText("Debug View");
    {
        int mode = m_renderer->GetPostFxMode(); // 1: Rendered, 2: DepthOnly
        const char* items[] = { "Rendered Image (1)", "DepthOnly (2)" };
        int idx = (mode == 2) ? 1 : 0;
        if (ImGui::Combo("View Mode", &idx, items, IM_ARRAYSIZE(items)))
        {
            m_renderer->SetPostFxMode(idx == 1 ? 2 : 1);
            anyChanged = true;
        }
    }

    // ---- Wire Rendering ----
    ImGui::SeparatorText("Debug");
    {
        bool wire = m_renderer->GetWireRendering();
        if (ImGui::Checkbox("Wire Rendering", &wire))
        {
            m_renderer->SetWireRendering(wire);
            // 이건 포스트FX 상수버퍼와는 무관할 수 있음
        }
    }

    // 포스트FX 상수 버퍼 더티 플래그
    /*if (anyChanged)
        m_renderer->MarkPostFxDirty();*/
}