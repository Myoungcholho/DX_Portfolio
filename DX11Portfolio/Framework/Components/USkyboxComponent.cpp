#include "Framework.h"
#include "USkyboxComponent.h"

void USkyboxComponent::Init()
{
    mName = "SkyboxComponent";

    renderProxy = make_shared<USkyboxRenderProxy>();
    renderProxy->Init(meshData);
    renderProxy->renderPass = GetRenderPass();
    renderProxy->bVisible = bVisible;                       // 이건 아직 유효하지 않음

    // GPU로드하면서 초기 셋팅값을 얻어옴
    materialConstsCPU = renderProxy->GetMaterialConstants();
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
    
}

shared_ptr<URenderProxy> USkyboxComponent::GetRenderProxy()
{
    RefreshConstantsCPU();

    renderProxy->SetMeshConstants(meshConstsCPU);
    renderProxy->SetMaterialConstants(materialConstsCPU);

    return renderProxy;
}
