#pragma once

#include "UPrimitiveComponent.h"

class UStaticMeshComponent : public UPrimitiveComponent
{
public:
    UStaticMeshComponent();
    ~UStaticMeshComponent() override = default;

public:
    void Init() override;
    void RefreshConstantsCPU() override;

public:
    ERenderPass GetRenderPass() const override { return ERenderPass::Opaque; }
    shared_ptr<URenderProxy> GetRenderProxy() override;

private:
    shared_ptr<UStaticMeshRenderProxy> renderProxy;
};