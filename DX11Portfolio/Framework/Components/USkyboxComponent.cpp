#include "Framework.h"
#include "USkyboxComponent.h"

void USkyboxComponent::Init()
{
    mName = "SkyboxComponent";

    renderProxy = make_shared<USkyboxRenderProxy>();
    renderProxy->Init(m_meshData);
    renderProxy->renderPass = GetRenderPass();
    renderProxy->bVisible = bVisible;                       // 이건 아직 유효하지 않음

    // GPU로드하면서 초기 셋팅값을 얻어옴
    m_materialConstsCPU = renderProxy->GetMaterialConstants();
}

void USkyboxComponent::RefreshConstantsCPU()
{
    UPrimitiveComponent::RefreshConstantsCPU();

}

void USkyboxComponent::OnGUI()
{

}

void USkyboxComponent::RenderNormal(ComPtr<ID3D11DeviceContext>& context)
{
    /*for (const auto& mesh : m_meshes)
    {
        context->GSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);
        context->Draw(mesh->vertexCount, 0);
    }*/
}

shared_ptr<URenderProxy> USkyboxComponent::GetRenderProxy()
{
    RefreshConstantsCPU();

    renderProxy->SetMeshConstants(m_meshConstsCPU);
    renderProxy->SetMaterialConstants(m_materialConstsCPU);

    return renderProxy;
}
