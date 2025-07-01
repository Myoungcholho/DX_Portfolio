#include "Pch.h"
#include "TestObjectQuad.h"

void TestObjectQuad::Initialize()
{
    transform = make_shared<Transform>();
    transform->SetPosition(Vector3(0, -5.f, 10.f));
    transform->SetScale(Vector3(1, 1, 1));

    std::vector<Vector4> controlPoints = { {-1.0f, 1.0f, 0.0 ,1.0f},
                                         {1.0f, 1.0f, 0.0, 1.0f},
                                         {-1.0f, -1.0f, 0.0,1.0f},
                                         {1.0f, -1.0f, 0.0,1.0f} };
    for (auto& cp : controlPoints) {
        cp.x *= 0.5f;
        cp.y *= 0.5f;
        cp.z = 0.5f; // 초기 위치 변경
    }

    // VS Buffer
    D3D::Get()->CreateVertexBuffer(controlPoints, m_vertexBuffer);
    m_indexCount = uint32_t(controlPoints.size());

    // cbuffer
    D3D::Get()->CreateConstantBuffer(m_constantData, m_constantBuffer);

    // Shader Compile
    vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0} 
    };
    D3D::Get()->CreateVertexShaderAndInputLayout(L"TessellatedQuad/TessellatedQuadVS.hlsl", inputElements, m_vertexShader,m_inputLayout);
    D3D::Get()->CreateHullShader(L"TessellatedQuad/TessellatedQuadHS.hlsl", m_hullShader);
    D3D::Get()->CreateDomainShader(L"TessellatedQuad/TessellatedQuadDS.hlsl", m_domainShader);
    D3D::Get()->CreatePixelShader(L"TessellatedQuad/TessellatedQuadPS.hlsl", m_pixelShader);
}

void TestObjectQuad::Destroy()
{

}

void TestObjectQuad::Tick()
{
    m_constantData.eyeWorld = CContext::Get()->GetCamera()->GetPosition();
    m_constantData.model = transform->GetWorldMatrix().Transpose();

    D3D::Get()->UpdateBuffer(m_constantData, m_constantBuffer);
}

void TestObjectQuad::UpdateGUI()
{

}

void TestObjectQuad::Render()
{
    ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();
    
    // VS
    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    // Hull shader
    context->HSSetShader(m_hullShader.Get(), 0, 0);
    context->HSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    // Domain shader
    context->DSSetShader(m_domainShader.Get(), 0, 0);
    context->DSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    // PS
    context->PSSetShader(m_pixelShader.Get(), 0, 0);
    context->PSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());
    

    // 토폴로지를 4개의 Control Point로 설정
    context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
    context->IASetInputLayout(m_inputLayout.Get());
    UINT stride = sizeof(Vector4);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

    context->Draw(m_indexCount, 0);

    // HS, DS를 사용하지 않는 다른 물체들을 위해 nullptr로 설정
    context->HSSetShader(nullptr, 0, 0);
    context->DSSetShader(nullptr, 0, 0);
}