#pragma once

#include "UPrimitiveComponent.h"

class UStaticMeshComponent : public UPrimitiveComponent
{
public:
    void Init() override;
    void RefreshConstantsCPU() override;
    void OnGUI() override;

public:
    ERenderPass GetRenderPass() const override { return ERenderPass::Opaque; }

public:
    //void Render(ComPtr<ID3D11DeviceContext>& context) override;
    //shared_ptr<URenderProxy> CreateRenderProxy() override;

    void RenderNormal(ComPtr<ID3D11DeviceContext>& context) override;

public:
    shared_ptr<URenderProxy> GetRenderProxy() override;

private:
    shared_ptr<UStaticMeshRenderProxy> renderProxy;
};