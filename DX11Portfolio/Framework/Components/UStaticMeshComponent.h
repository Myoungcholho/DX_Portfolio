#pragma once

#include "UPrimitiveComponent.h"

class UStaticMeshComponent : public UPrimitiveComponent
{
public:
    void Init() override;
    void UpdateConstantBuffers(
        ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context) override;
    void OnGUI() override;

public:
    ERenderPass GetRenderPass() const override { return ERenderPass::Opaque; }

public:
    void Render(ComPtr<ID3D11DeviceContext>& context) override;
    void RenderNormal(ComPtr<ID3D11DeviceContext>& context) override;

};