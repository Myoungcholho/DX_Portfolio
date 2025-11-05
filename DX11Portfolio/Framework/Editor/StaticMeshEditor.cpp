#include "Framework.h"
#include "StaticMeshEditor.h"

PrimitiveEditor::PrimitiveEditor(UPrimitiveComponent* target) : mTarget(target)
{
	RefreshBuffersFromTarget();
}

void PrimitiveEditor::SetTarget(UPrimitiveComponent* target)
{
	mTarget = target;
	RefreshBuffersFromTarget();
}

void PrimitiveEditor::OnGUI()
{
    if (!mTarget) return;

    ImGui::PushID(mTarget);

    // TransformEditor와 동일한 타이틀 규칙
    const char* title = GetName().empty() ? "Static Mesh" : GetName().c_str();
    ImGui::SeparatorText(title);

    bool dirty = false;

    // ── Material ───────────────────────────────────────────
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
        dirty |= ImGui::SliderFloat("Metallic", &metallicFactor, 0.0f, 1.0f);
        dirty |= ImGui::SliderFloat("Roughness", &roughnessFactor, 0.0f, 1.0f);

        // 필드 타입이 int(0/1)라면 CheckboxFlags, bool이라면 Checkbox 사용
        dirty |= ImGui::CheckboxFlags("AlbedoTexture", &useAlbedoMap, 1);
        dirty |= ImGui::CheckboxFlags("EmissiveTexture", &useEmissiveMap, 1);
        dirty |= ImGui::CheckboxFlags("Use NormalMapping", &useNormalMap, 1);
        dirty |= ImGui::CheckboxFlags("Use AO", &useAOMap, 1);
        dirty |= ImGui::CheckboxFlags("Use MetallicMap", &useMetallicMap, 1);
        dirty |= ImGui::CheckboxFlags("Use RoughnessMap", &useRoughnessMap, 1);
    }

    // ── Mesh / Height ──────────────────────────────────────
    if (ImGui::CollapsingHeader("Mesh / Height", ImGuiTreeNodeFlags_DefaultOpen))
    {
        dirty |= ImGui::CheckboxFlags("Use HeightMapping", &useHeightMap, 1);
        dirty |= ImGui::SliderFloat("HeightScale", &heightScale, 0.0f, 0.1f);
    }

    // ── Debug ─────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Debug"))
    {
        dirty |= ImGui::Checkbox("Draw Normals", &drawNormals);
    }

    // 변경 반영
    if (dirty)
        ApplyBuffersToTarget();

    ImGui::PopID();
}

void PrimitiveEditor::RefreshBuffersFromTarget()
{
	if (!mTarget) return;

    const MaterialConstants& material = mTarget->GetMaterialConstants();
    metallicFactor = material.metallicFactor;
    roughnessFactor = material.roughnessFactor;
    useAlbedoMap = material.useAlbedoMap;
    useEmissiveMap = material.useEmissiveMap;
    useNormalMap = material.useNormalMap;
    useAOMap = material.useAOMap;
    useMetallicMap = material.useMetallicMap;
    useRoughnessMap = material.useRoughnessMap;

    const WorldInvConstantBuffer& mesh = mTarget->GetMeshConstants();
    useHeightMap = mesh.useHeightMap;
    heightScale = mesh.heightScale;

    drawNormals = mTarget->GetNormalDraws();
}

void PrimitiveEditor::ApplyBuffersToTarget()
{
	if (!mTarget) return;

    // 1) 머티리얼 상수
    MaterialConstants material = mTarget->GetMaterialConstants(); // 복사
    material.metallicFactor = metallicFactor;
    material.roughnessFactor = roughnessFactor;
    material.useAlbedoMap = useAlbedoMap;
    material.useEmissiveMap = useEmissiveMap;
    material.useNormalMap = useNormalMap;
    material.useAOMap = useAOMap;
    material.useMetallicMap = useMetallicMap;
    material.useRoughnessMap = useRoughnessMap;
    mTarget->SetMaterialConstants(material);

    // 2) 메시 상수
    WorldInvConstantBuffer mesh = mTarget->GetMeshConstants(); // 복사
    mesh.useHeightMap = useHeightMap;
    mesh.heightScale = heightScale;
    mTarget->SetMeshConstants(mesh); // setter로 반영

    // 3) 디버그 플래그
    mTarget->SetNormalDraws(drawNormals);
}