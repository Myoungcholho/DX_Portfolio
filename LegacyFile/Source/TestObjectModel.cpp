#include "Pch.h"
#include "TestObject/TestObjectModel.h"

void TestObjectModel::Initialize()
{
    transform = make_shared<Transform>();
    transform->SetScale(Vector3(1.8f));
    transform->SetRotation(Vector3(0.0f));
    transform->SetPosition(Vector3(4.0f, -3.0f, 10.0f));

	//auto meshes = GeomtryGenerator::ReadFromFile("Zelda/", "zeldaPosed001.fbx");
    vector<MeshData> meshes; // = GeomtryGenerator::ReadFromFileModel("Fallen/", "SK_Fallen_Samurai.fbx");

    ComPtr<ID3D11Buffer> VertexConstantBuffer;
    ComPtr<ID3D11Buffer> PixelConstantBuffer;

    WorldMatrixData.World = Matrix();
    WorldMatrixData.InvTranspose = Matrix();
    D3D::Get()->CreateConstantBuffer(WorldMatrixData, VertexConstantBuffer);
    D3D::Get()->CreateConstantBuffer(BasicPixelConstantBufferData, PixelConstantBuffer);
    D3D::Get()->CreateConstantBuffer(RimParamsData, RimParamsCBuffer);

    for (const auto& meshData : meshes)
    {
        shared_ptr<FMesh> newMesh = std::make_shared<FMesh>();
        
        D3D::Get()->CreateVertexBuffer(meshData.vertices, newMesh->VertexBuffer);
        newMesh->IndexCount = UINT(meshData.indices.size());
        newMesh->VertexCount = UINT(meshData.vertices.size());
        D3D::Get()->CreateIndexBuffer(meshData.indices, newMesh->IndexBuffer);

        if (!meshData.textureFilename.empty())
        {
            newMesh->Texture = std::make_shared<CTexture>(meshData.textureFilename,false);
        }

        newMesh->VSCBuffer = VertexConstantBuffer;
        newMesh->PSCBuffer = PixelConstantBuffer;
        

        this->Meshs.push_back(newMesh);
    }

    vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = 
    {
       {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
        D3D11_INPUT_PER_VERTEX_DATA, 0},
       {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
        D3D11_INPUT_PER_VERTEX_DATA, 0},
       {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
        D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D::Get()->CreateVertexShaderAndInputLayout(L"VS_Basic.hlsl", basicInputElements, BasicVertexShader, InputLayout);
    D3D::Get()->CreatePixelShader(L"PS_Basic.hlsl", BasicPixelShader);
    
    // 노멀벡터 로직 아래에 추가
    NormalInit(meshes,basicInputElements);
}

void TestObjectModel::Destroy()
{

}

void TestObjectModel::UpdateGUI()
{
    ImGui::Begin(Name.c_str());
    {
        //ImGui::Checkbox("Use Texture", &BasicPixelConstantBufferData.UseTexture);
        //ImGui::SliderFloat3("MaterialDiffuse", &BasicPixelConstantBufferData.material.diffuse.x ,0.0f,1.0f);
        //ImGui::SliderFloat3("MaterialSpecular", &BasicPixelConstantBufferData.material.specular.x, 0.0f, 1.0f);
        
        ImGui::SliderFloat3("Rim Color", &RimParamsData.RimColor.x,0.0f,1.0f);
        ImGui::SliderFloat("Rim Power", &RimParamsData.RimPower,0.01f,10.0f);
        ImGui::SliderFloat("Rim Stength", &RimParamsData.RimStrength, 0.0f, 10.0f);
        ImGui::Checkbox("Rim Smoothstep", &RimParamsData.useSmoothstep);
        ImGui::Checkbox("Draw Normal", &m_drawNormals);

        // Transform 조절
        Vector3 pos = transform->GetPosition();
        if (ImGui::SliderFloat3("Position", &pos.x, -50.0f, 50.0f))
            transform->SetPosition(pos);

        Vector3 rot = transform->GetRotation();
        if (ImGui::SliderFloat3("Rotation", &rot.x, -180.0f, 180.0f))
            transform->SetRotation(rot);

        Vector3 scale = transform->GetScale();
        if (ImGui::SliderFloat3("Scale", &scale.x, 0.1f, 5.0f))
            transform->SetScale(scale);
    }
    ImGui::End();
}

void TestObjectModel::Tick()
{
    // 1. 값 업데이트
    WorldMatrixData.World = transform->GetWorldMatrix().Transpose();

    WorldMatrixData.InvTranspose = WorldMatrixData.World;
    WorldMatrixData.InvTranspose.Translation(Vector3(0.0f));
    WorldMatrixData.InvTranspose = WorldMatrixData.InvTranspose.Transpose().Invert();

    // 2. CPU -> GPU
    if (Meshs[0])
    {
        D3D::Get()->UpdateBuffer(WorldMatrixData, Meshs[0]->VSCBuffer);
    }

    // View행렬을 GPU에서 변환해서 사용해도 되지만, 역행렬 계산 필요하므로 정보를 따로 전달
    BasicPixelConstantBufferData.EyeWorld = CContext::Get()->GetCamera()->GetPosition();

    // 조명을 일단 비워두고
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        
    }

    if (Meshs[0])
    {
        D3D::Get()->UpdateBuffer(BasicPixelConstantBufferData, Meshs[0]->PSCBuffer);
    }

    // Update RimLight Data to Buffer
    D3D::Get()->UpdateBuffer(RimParamsData, RimParamsCBuffer);
}

void TestObjectModel::Render()
{
    // 3. 메쉬 ConstantBuffer 전달
    UINT stride = sizeof(FVertex);
    UINT offset = 0;
    for (const auto& mesh : Meshs)
    {
        // 1. 셰이더 설정
        D3D::Get()->GetDeviceContext()->VSSetShader(BasicVertexShader.Get(), 0, 0);
        D3D::Get()->GetDeviceContext()->PSSetShader(BasicPixelShader.Get(), 0, 0);

        // 2. Sampler 설정
        D3D::Get()->GetDeviceContext()->PSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());

        // Rim(b2)
        D3D::Get()->GetDeviceContext()->PSSetConstantBuffers(2, 1, RimParamsCBuffer.GetAddressOf());

        D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(0, 1, mesh->VSCBuffer.GetAddressOf());
        D3D::Get()->GetDeviceContext()->PSSetConstantBuffers(0, 1, mesh->PSCBuffer.GetAddressOf());

        ID3D11ShaderResourceView* pixelResources[3] =
        {
            mesh->Texture->GetSRV(),
            Engine::Get()->GetTextureManager()->Get(L"SkyBox_Diffuse"),
            Engine::Get()->GetTextureManager()->Get(L"SkyBox_Specular"),
        };
        D3D::Get()->GetDeviceContext()->PSSetShaderResources(0, 3, pixelResources);

        D3D::Get()->GetDeviceContext()->IASetInputLayout(InputLayout.Get());
        D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
        D3D::Get()->GetDeviceContext()->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        D3D::Get()->GetDeviceContext()->DrawIndexed(mesh->IndexCount, 0, 0);

        // 노멀 벡터 그리기
        if (m_drawNormals) 
        {
            // VS
            D3D::Get()->GetDeviceContext()->VSSetShader(m_normalVertexShader.Get(), 0, 0);

            // GS
            D3D::Get()->GetDeviceContext()->GSSetShader(m_normalGeometryShdaer.Get(), 0, 0);
            ID3D11Buffer* pptr[1] = { Meshs[0]->VSCBuffer.Get() };
            D3D::Get()->GetDeviceContext()->GSSetConstantBuffers(0, 1, pptr);

            // PS
            D3D::Get()->GetDeviceContext()->PSSetShader(m_normalPixelShader.Get(), 0, 0);

            // IA
            D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
            D3D::Get()->GetDeviceContext()->Draw(mesh->VertexCount, 0);

            D3D::Get()->GetDeviceContext()->GSSetShader(nullptr, 0, 0);
        }
    }

}

void TestObjectModel::NormalInit(vector<MeshData>& meshes, vector<D3D11_INPUT_ELEMENT_DESC>& basicInputElements)
{
    // Normal용 VS/GS/PS Shdaer 컴파일
    D3D::Get()->CreateVertexShaderAndInputLayout(
        L"NormalVertexShader.hlsl", basicInputElements, m_normalVertexShader,
        InputLayout);
    D3D::Get()->CreateGeometryShader(L"NormalGeometryShader.hlsl", m_normalGeometryShdaer);
    D3D::Get()->CreatePixelShader(L"NormalPixelShader.hlsl", m_normalPixelShader);
}