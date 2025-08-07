#include "Framework.h"
#include "UStaticMeshComponent.h"

void UStaticMeshComponent::Init()
{
    renderProxy = make_shared<UStaticMeshRenderProxy>();
    renderProxy->Init(m_meshData);
    renderProxy->renderPass = GetRenderPass();
    renderProxy->bVisible = bVisible;

}

void UStaticMeshComponent::RefreshConstantsCPU()
{
    UPrimitiveComponent::RefreshConstantsCPU();
    //D3D11Utils:   :UpdateBuffer(device, context, m_materialConstsCPU, m_materialConstsGPU);
}

void UStaticMeshComponent::OnGUI()
{
    if (ImGui::TreeNode("Material"))
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

        //ImGui::Checkbox("Draw Normals", &m_mainObj->m_drawNormals);

        ImGui::TreePop();
    }
}

void UStaticMeshComponent::RenderNormal(ComPtr<ID3D11DeviceContext>& context)
{
    /*for (const auto& mesh : m_meshes)
    {
        context->GSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);
        context->Draw(mesh->vertexCount, 0);
    }*/
}

shared_ptr<URenderProxy> UStaticMeshComponent::GetRenderProxy()
{
    RefreshConstantsCPU();

    renderProxy->SetMeshConstants(m_meshConstsCPU);
    renderProxy->SetMaterialConstants(m_materialConstsCPU);

    return renderProxy;
}
