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
    shared_ptr<URenderProxy> GetRenderProxy() override;

private:
    shared_ptr<UStaticMeshRenderProxy> renderProxy;
};