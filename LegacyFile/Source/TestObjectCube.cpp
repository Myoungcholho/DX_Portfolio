#include "Pch.h"
#include "TestObjectCube.h"

#include <tuple>
#include <vector>

#ifdef LegacyObject

auto TestObjectCube::MakeBox()
{
    vector<Vector3> positions;
    vector<Vector3> colors;

    const float scale = 1.0f;

    // 윗면
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));


    // 아랫면
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));


    // 앞면
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));

    // 뒷면
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));


    // 왼쪽
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));
    colors.push_back(Vector3(0.7f, 0.7f, 0.7f));


    // 오른쪽
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));


    vector<FVertexPC> vertices;
    for (size_t i = 0; i < positions.size(); i++) {
        FVertexPC v;
        v.position = positions[i];
        v.color = colors[i];
        vertices.push_back(v);
    }

    vector<uint16_t> indices = {
        0,  1,  2,  0,  2,  3,  // 윗면
        4,  5,  6,  4,  6,  7,  // 아랫면
        8,  9,  10, 8,  10, 11, // 앞면
        12, 13, 14, 12, 14, 15, // 뒷면
        16, 17, 18, 16, 18, 19, // 왼쪽
        20, 21, 22, 20, 22, 23  // 오른쪽
    };

    return std::make_tuple(vertices, indices);
}

void TestObjectCube::Initialize()
{
	// Gemotry정의
    auto result = MakeBox();
    auto& vertices = std::get<0>(result);
    auto& indices = std::get<1>(result);

    // 버텍스 버퍼 만들기
    D3D::Get()->CreateVertexBuffer(vertices, m_vertexBuffer);

    // 인덱스 버퍼 만들기
    m_indexCount = UINT(indices.size());
    D3D::Get()->CreateIndexBuffer(indices, m_indexBuffer);

    // ConstantBuffer 만들기
    m_constantBufferData.World = Matrix();
    m_constantBufferData.InvTranspose = Matrix();

    D3D::Get()->CreateConstantBuffer(m_constantBufferData, m_constantBuffer);

    vector<D3D11_INPUT_ELEMENT_DESC> inputElements = 
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D::Get()->CreateVertexShaderAndInputLayout(L"PlaneVertexShader.hlsl", inputElements,
        m_colorVertexShader, m_colorInputLayout);

    D3D::Get()->CreatePixelShader(L"PlanePixelShader.hlsl", m_colorPixelShader);
}

void TestObjectCube::Destroy()
{

}

void TestObjectCube::UpdateGUI()
{

}

void TestObjectCube::Tick()
{
    // 1. 모델 행렬 (자기 자신이 관리)
    m_constantBufferData.World =
        Matrix::CreateScale(1.0f) * Matrix::CreateRotationY(0.0f) *
        Matrix::CreateTranslation(Vector3(-6.0f, -4.0f, 15.0f));
    m_constantBufferData.World = m_constantBufferData.World.Transpose();

    m_constantBufferData.InvTranspose = m_constantBufferData.World;
    m_constantBufferData.InvTranspose.Translation(Vector3(0.0f));
    m_constantBufferData.InvTranspose = m_constantBufferData.InvTranspose.Transpose().Invert();

    // 4. GPU로 상수버퍼 전송
    D3D::Get()->UpdateBuffer(m_constantBufferData, m_constantBuffer);
}

void TestObjectCube::Render()
{
    D3D::Get()->GetDeviceContext()->VSSetShader(m_colorVertexShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    D3D::Get()->GetDeviceContext()->PSSetShader(m_colorPixelShader.Get(), 0, 0);

    UINT stride = sizeof(FVertexPC);
    UINT offset = 0;
    D3D::Get()->GetDeviceContext()->IASetInputLayout(m_colorInputLayout.Get());
    D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    D3D::Get()->GetDeviceContext()->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D::Get()->GetDeviceContext()->DrawIndexed(m_indexCount, 0, 0);
}
#endif