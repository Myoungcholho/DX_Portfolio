#include "Framework.h"
#include "UStaticMeshComponent.h"

UStaticMeshComponent::UStaticMeshComponent()
{
    mName = "StaticMeshComponent";
}

void UStaticMeshComponent::Init()
{
    renderProxy = make_shared<UStaticMeshRenderProxy>();
    renderProxy->Init(meshData);
    renderProxy->renderPass = GetRenderPass();
    renderProxy->bVisible = bVisible;

    materialConstsCPU = renderProxy->GetMaterialConstants();
}

void UStaticMeshComponent::RefreshConstantsCPU()
{
    UPrimitiveComponent::RefreshConstantsCPU();
}

shared_ptr<URenderProxy> UStaticMeshComponent::GetRenderProxy()
{
    RefreshConstantsCPU();

    renderProxy->SetMeshConstants(meshConstsCPU);
    renderProxy->SetMaterialConstants(materialConstsCPU);
    renderProxy->SetDrawNormal(drawNormals);

    return renderProxy;
}
