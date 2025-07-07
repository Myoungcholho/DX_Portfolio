#include "Framework.h"
#include "BillboardRenderer_GS.h"

void BillboardRenderer_GS::Initialize()
{
    // CBuffer Create
    m_constantData.width = 2.4f;
    m_constantData.EyeWorld = CContext::Get()->GetCamera()->GetPosition();

    D3D::Get()->CreateConstantBuffer(m_constantData, m_constantBuffer);

    // GS Shader
    D3D::Get()->CreateGeometryShader(L"Billboards/BillboardPointsGeometryShader.hlsl", m_geometryShader);

    // VS Shader
    vector<D3D11_INPUT_ELEMENT_DESC> inputElements = 
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    D3D::Get()->CreateVertexShaderAndInputLayout(
        L"Billboards/BillboardPointsVertexShader.hlsl", inputElements, m_vertexShader, m_inputLayout);

    // PS Shader
    D3D::Get()->CreatePixelShader(L"Billboards/BillboardPointsPixelShader.hlsl", m_pixelShader);

    // Textures
    vector<string> filenames =
    {
        "../../../_Textures/TreeBillboards/1.png",
        "../../../_Textures/TreeBillboards/2.png",
        "../../../_Textures/TreeBillboards/3.png",
        "../../../_Textures/TreeBillboards/4.png",
        "../../../_Textures/TreeBillboards/5.png"
    };
    
    D3D::Get()->CreateTextureArray(filenames, m_texArray, m_texArraySRV);
}

void BillboardRenderer_GS::Tick()
{
    m_constantData.EyeWorld = CContext::Get()->GetCamera()->GetPosition();

    D3D::Get()->UpdateBuffer(m_constantData, m_constantBuffer);
}

void BillboardRenderer_GS::Render(BillboardInstance *data)
{
    ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();

    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->PSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());
    context->PSSetShader(m_pixelShader.Get(), 0, 0);
    context->PSSetShaderResources(0, 1, m_texArraySRV.GetAddressOf());

    context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    context->GSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    context->GSSetShader(m_geometryShader.Get(), 0, 0);

    context->IASetInputLayout(m_inputLayout.Get());

    UINT stride = sizeof(BillboardPoint);
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, data->GetVertexBuffer().GetAddressOf(),&stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    context->Draw(data->GetPointCount(), 0);

    context->GSSetShader(nullptr, 0, 0);
}