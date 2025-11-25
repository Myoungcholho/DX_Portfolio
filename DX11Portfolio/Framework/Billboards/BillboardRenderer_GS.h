#pragma once

#include "IBillboardRenderer.h"

using Microsoft::WRL::ComPtr;

class BillboardRenderer_GS : public IBillboardRenderer
{
public:
    void Initialize() override;
    void Tick() override;
    void Render(BillboardInstance* data) override;

private:
    BillboardPointCBuffer constantData;
    
private:
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11GeometryShader> geometryShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;

    ComPtr<ID3D11Buffer> constantBuffer;
    ComPtr<ID3D11ShaderResourceView> textureArraySRV;

    ComPtr<ID3D11Texture2D> texArray;
    ComPtr<ID3D11ShaderResourceView> texArraySRV;
};