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
    BillboardPointCBuffer m_constantData;
    
private:
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11GeometryShader> m_geometryShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;

    ComPtr<ID3D11Buffer> m_constantBuffer;
    ComPtr<ID3D11ShaderResourceView> m_textureArraySRV;

    ComPtr<ID3D11Texture2D> m_texArray;
    ComPtr<ID3D11ShaderResourceView> m_texArraySRV;
};