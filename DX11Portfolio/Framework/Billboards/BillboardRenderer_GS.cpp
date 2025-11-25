#include "Framework.h"
#include "BillboardRenderer_GS.h"

void BillboardRenderer_GS::Initialize()
{
    //// CBuffer Create
    //constantData.width = 2.4f;
    //constantData.EyeWorld = CContext::Get()->GetCamera()->GetPosition();

    //D3D::Get()->CreateConstantBuffer(constantData, constantBuffer);

    //// GS Shader
    //D3D::Get()->CreateGeometryShader(L"Billboards/BillboardPointsGeometryShader.hlsl", geometryShader);

    //// VS Shader
    //vector<D3D11_INPUT_ELEMENT_DESC> inputElements = 
    //{
    //    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    //};
    //D3D::Get()->CreateVertexShaderAndInputLayout(
    //    L"Billboards/BillboardPointsVertexShader.hlsl", inputElements, vertexShader, inputLayout);

    //// PS Shader
    //D3D::Get()->CreatePixelShader(L"Billboards/BillboardPointsPixelShader.hlsl", pixelShader);

    //// Textures
    //vector<string> filenames =
    //{
    //    "../../../_Textures/TreeBillboards/1.png",
    //    "../../../_Textures/TreeBillboards/2.png",
    //    "../../../_Textures/TreeBillboards/3.png",
    //    "../../../_Textures/TreeBillboards/4.png",
    //    "../../../_Textures/TreeBillboards/5.png"
    //};
    //
    //D3D::Get()->CreateTextureArray(filenames, true, texArray, texArraySRV);
}

void BillboardRenderer_GS::Tick()
{
    //constantData.EyeWorld = CContext::Get()->GetCamera()->GetPosition();

    //D3D::Get()->UpdateBuffer(constantData, constantBuffer);
}

void BillboardRenderer_GS::Render(BillboardInstance *data)
{
    //ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();

    //context->VSSetShader(vertexShader.Get(), 0, 0);
    //context->PSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());
    //context->PSSetShader(pixelShader.Get(), 0, 0);
    //context->PSSetShaderResources(0, 1, texArraySRV.GetAddressOf());

    //context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    //context->GSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    //context->GSSetShader(geometryShader.Get(), 0, 0);

    //context->IASetInputLayout(inputLayout.Get());

    //UINT stride = sizeof(BillboardPoint);
    //UINT offset = 0;

    //context->IASetVertexBuffers(0, 1, data->GetVertexBuffer().GetAddressOf(),&stride, &offset);
    //context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    //context->Draw(data->GetPointCount(), 0);

    //context->GSSetShader(nullptr, 0, 0);
}