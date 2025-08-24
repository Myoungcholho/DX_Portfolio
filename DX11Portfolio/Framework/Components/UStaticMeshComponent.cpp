#include "Framework.h"
#include "UStaticMeshComponent.h"

UStaticMeshComponent::UStaticMeshComponent()
{
    mName = "StaticMeshComponent";
}

void UStaticMeshComponent::Init()
{
    renderProxy = make_shared<UStaticMeshRenderProxy>();
    renderProxy->Init(m_meshData);
    renderProxy->renderPass = GetRenderPass();
    renderProxy->bVisible = bVisible;

    m_materialConstsCPU = renderProxy->GetMaterialConstants();
}

void UStaticMeshComponent::RefreshConstantsCPU()
{
    UPrimitiveComponent::RefreshConstantsCPU();
    //D3D11Utils:   :UpdateBuffer(device, context, m_materialConstsCPU, m_materialConstsGPU);
}

void UStaticMeshComponent::OnGUI()
{
    string str = Owner->GetName() + "Material";
    ImGui::Begin("Actors");
    if (ImGui::TreeNode(str.c_str()))
    {
        int flag = 0;

        flag += ImGui::SliderFloat(
            "Metallic", &m_materialConstsCPU.metallicFactor, 0.0f,
            1.0f);
        flag += ImGui::SliderFloat(
            "Roughness", &m_materialConstsCPU.roughnessFactor, 0.0f,
            1.0f);
        flag += ImGui::CheckboxFlags(
            "AlbedoTexture", &m_materialConstsCPU.useAlbedoMap, 1);
        flag += ImGui::CheckboxFlags(
            "EmissiveTexture", &m_materialConstsCPU.useEmissiveMap,
            1);
        flag += ImGui::CheckboxFlags(
            "Use NormalMapping", &m_materialConstsCPU.useNormalMap,
            1);
        flag += ImGui::CheckboxFlags(
            "Use AO", &m_materialConstsCPU.useAOMap, 1);
        flag += ImGui::CheckboxFlags(
            "Use HeightMapping", &m_meshConstsCPU.useHeightMap, 1);
        flag += ImGui::SliderFloat(
            "HeightScale", &m_meshConstsCPU.heightScale, 0.0f, 0.1f);
        flag += ImGui::CheckboxFlags(
            "Use MetallicMap", &m_materialConstsCPU.useMetallicMap,
            1);
        flag += ImGui::CheckboxFlags(
            "Use RoughnessMap", &m_materialConstsCPU.useRoughnessMap,
            1);
        

        if (flag)
        {
            RefreshConstantsCPU();
        }

        ImGui::Checkbox("Draw Normals", &m_drawNormals);

        ImGui::TreePop();
    }
    ImGui::End();
}

shared_ptr<URenderProxy> UStaticMeshComponent::GetRenderProxy()
{
    RefreshConstantsCPU();

    renderProxy->SetMeshConstants(m_meshConstsCPU);
    renderProxy->SetMaterialConstants(m_materialConstsCPU);
    renderProxy->SetDrawNormal(m_drawNormals);

    return renderProxy;
}
