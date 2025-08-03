#include "Pch.h"
#include "TestObjectCylinder.h"

auto TestObjectCylinder::MakeCylinder(const float bottomRadius, const float topRadius, float height, int sliceCount)
{
    using namespace DirectX;
    using DirectX::SimpleMath::Matrix;
    using DirectX::SimpleMath::Vector3;

    const float dTheta = -XM_2PI / float(sliceCount);

    vector<FVertexPNCU> vertices;
    vector<uint16_t> indices;

    for (int i = 0; i <= sliceCount; i++) 
    {
        FVertexPNCU v;

        // TODO: 작성 (텍스춰 좌표계, 버텍스 노멀 필요)
        v.position = Vector3(cos(dTheta * i), 0.0f, sin(dTheta * i));
        v.uv = Vector2(float(i) / sliceCount, 1.0f);
        v.normal = Vector3(cos(dTheta * i), 0.0f, sin(dTheta * i));
        v.normal.Normalize();

        vertices.push_back(v);
    }

    // 옆면의 맨 위 버텍스들 (인덱스 sliceCount 이상 2 * sliceCount 미만)
    for (int i = 0; i <= sliceCount; i++) {
        FVertexPNCU v;

        // TODO: 작성 (텍스춰 좌표계, 버텍스 노멀 필요)
        v.position = Vector3(cos(dTheta * i), 1.0f, sin(dTheta * i));
        v.uv = Vector2(float(i) / sliceCount, 0.0f);
        v.normal = Vector3(cos(dTheta * i), 0.0f, sin(dTheta * i));
        v.normal.Normalize();

        vertices.push_back(v);
    }

    // 바닥 중심 정점 추가
    FVertexPNCU bottomCenter;
    bottomCenter.position = Vector3(0.0f, 0.0f, 0.0f);
    bottomCenter.uv = Vector2(0.5f, 0.5f);
    bottomCenter.normal = Vector3(0.0f, -1.0f, 0.0f);
    vertices.push_back(bottomCenter);
    int bottomCenterIndex = (int)vertices.size() - 1;
    // 윗면 중심 정점 추가
    FVertexPNCU topCenter;
    topCenter.position = Vector3(0.0f, 1.0f, 0.0f);
    topCenter.uv = Vector2(0.5f, 0.5f);
    topCenter.normal = Vector3(0.0f, 1.0f, 0.0f);
    vertices.push_back(topCenter);
    int topCenterIndex = (int)vertices.size() - 1;

    int baseTopIndex = sliceCount + 1;
    for (int i = 0; i < sliceCount; i++)
    {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 1 + baseTopIndex);

        indices.push_back(i);
        indices.push_back(i + 1 + baseTopIndex);
        indices.push_back(i + baseTopIndex);
    }

    for (int i = 0; i < sliceCount; ++i)
    {
        int current = i;
        int next = (i + 1) % sliceCount;
        indices.push_back(bottomCenterIndex);
        indices.push_back(next);
        indices.push_back(current);
    }

    for (int i = 0; i < sliceCount; ++i)
    {
        int current = baseTopIndex + i;
        int next = baseTopIndex + ((i + 1) % sliceCount);
        indices.push_back(topCenterIndex);
        indices.push_back(current);
        indices.push_back(next);
    }

    return make_pair(vertices,indices);
}

void TestObjectCylinder::Initialize()
{
    auto result = MakeCylinder(30, 30, 30, 30);
    vector<FVertexPNCU>& vertices = get<0>(result);
    vector<uint16_t>& indices = get<1>(result);

    D3D::Get()->CreateVertexBuffer(vertices, Mesh.VertexBuffer);
    Mesh.IndexCount = UINT(indices.size());
    D3D::Get()->CreateIndexBuffer(indices, Mesh.IndexBuffer);

    ConstantBufferData.World = Matrix();
    ConstantBufferData.InvTranspose = Matrix();

    D3D::Get()->CreateConstantBuffer(ConstantBufferData, Mesh.VSCBuffer);

    vector<D3D11_INPUT_ELEMENT_DESC> inputElements =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36 , D3D11_INPUT_PER_VERTEX_DATA, 0}, // 8
    };

    D3D::Get()->CreateVertexShaderAndInputLayout(L"FloorVS.hlsl", inputElements,
        VertexShader, InputLayout);
    D3D::Get()->CreatePixelShader(L"FloorPS.hlsl", PixelShader);

    texture = new CTexture(L"cat2.png");
}

void TestObjectCylinder::Destroy()
{

}

void TestObjectCylinder::UpdateGUI()
{

}

void TestObjectCylinder::Tick()
{
    ConstantBufferData.World =
        Matrix::CreateScale(1.0f) * Matrix::CreateRotationY(0.0f) *
        Matrix::CreateTranslation(Vector3(-5.0f, -5.0f, 10.0f));
    ConstantBufferData.World = ConstantBufferData.World.Transpose();
    
    ConstantBufferData.InvTranspose = ConstantBufferData.World;
    ConstantBufferData.InvTranspose.Translation(Vector3(0.0f));
    ConstantBufferData.InvTranspose = ConstantBufferData.InvTranspose.Transpose().Invert();

    D3D::Get()->UpdateBuffer(ConstantBufferData, Mesh.VSCBuffer);
}

void TestObjectCylinder::Render()
{
    D3D::Get()->GetDeviceContext()->VSSetShader(VertexShader.Get(), 0, 0);
    D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(0, 1, Mesh.VSCBuffer.GetAddressOf());
    D3D::Get()->GetDeviceContext()->PSSetShader(PixelShader.Get(), 0, 0);

    // 텍스처 정보
    ID3D11ShaderResourceView* pixelResources[1] = { texture->GetSRV() };
    D3D::Get()->GetDeviceContext()->PSSetShaderResources(0, 1, pixelResources);
    D3D::Get()->GetDeviceContext()->PSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());


    UINT stride = sizeof(FVertexPNCU);
    UINT offset = 0;
    D3D::Get()->GetDeviceContext()->IASetInputLayout(InputLayout.Get());
    D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, Mesh.VertexBuffer.GetAddressOf(), &stride, &offset);
    D3D::Get()->GetDeviceContext()->IASetIndexBuffer(Mesh.IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D::Get()->GetDeviceContext()->DrawIndexed(Mesh.IndexCount, 0, 0);
}