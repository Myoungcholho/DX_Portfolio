#include "Pch.h"
#include "MeshGroup.h"


MeshGroup::MeshGroup(std::string basePath, std::string filename,string name)
{
	this->basePath = basePath;
	this->filename = filename;

    meshes = GeomtryGenerator::ReadFromFileModel(basePath, filename);

    this->Name += name;

    transform = make_shared<Transform>();
}

MeshGroup::MeshGroup(const vector<PBRMeshData>& meshes,string name)
{
    this->meshes = meshes;

    this->Name += name;

    transform = make_shared<Transform>();
}

void MeshGroup::Initialize()
{
    // ConstantBuffer 
    worldInvConstantBufferData.World = Matrix();
    worldInvConstantBufferData.InvTranspose = Matrix();

    D3D::Get()->CreateConstantBuffer(m_basicVertexConstData, m_vertexConstantBuffer);
    D3D::Get()->CreateConstantBuffer(m_basicPixelConstData, m_pixelConstantBuffer);
    D3D::Get()->CreateConstantBuffer(worldInvConstantBufferData, worldInvConstantBuffer);
    D3D::Get()->CreateConstantBuffer(lightConstantBufferData, lightConstantBuffer);

	// Mesh Read
    for (const auto& meshData : meshes) 
    {
        auto newMesh = std::make_shared<Mesh>();
        D3D::Get()->CreateVertexBuffer(meshData.vertices, newMesh->vertexBuffer);
        newMesh->m_indexCount = UINT(meshData.indices.size());
        newMesh->m_vertexCount = UINT(meshData.vertices.size());

        D3D::Get()->CreateIndexBuffer(meshData.indices, newMesh->indexBuffer);

        if (!meshData.albedoTextureFilename.empty()) 
        {
            D3D::Get()->CreateTexture(meshData.albedoTextureFilename, true, newMesh->albedoTexture, newMesh->albedoSRV);
            m_basicPixelConstData.useAlbedoMap = true;
        }

        if (!meshData.emissiveTextureFilename.empty()) 
        {
            D3D::Get()->CreateTexture(meshData.emissiveTextureFilename, true, newMesh->emissiveTexture, newMesh->emissiveSRV);
            m_basicPixelConstData.useEmissiveMap = true;
        }

        if (!meshData.normalTextureFilename.empty()) 
        {
            D3D::Get()->CreateTexture(meshData.normalTextureFilename, false, newMesh->normalTexture, newMesh->normalSRV);
            m_basicPixelConstData.useNormalMap = true;
        }

        if (!meshData.heightTextureFilename.empty()) 
        {
            D3D::Get()->CreateTexture( meshData.heightTextureFilename, false, newMesh->heightTexture, newMesh->heightSRV);
            m_basicVertexConstData.useHeightMap = true;
        }

        if (!meshData.aoTextureFilename.empty()) 
        {
            D3D::Get()->CreateTexture(meshData.aoTextureFilename, false, newMesh->aoTexture, newMesh->aoSRV);
            m_basicPixelConstData.useAOMap = true;
        }

        // GLTF 방식으로 Metallic과 Roughness를 한 텍스춰에 넣음
        // Green : Roughness, Blue : Metallic(Metalness)
        if (!meshData.metallicTextureFilename.empty() || !meshData.roughnessTextureFilename.empty()) 
        {
            D3D::Get()->CreateMetallicRoughnessTexture
            (
                meshData.metallicTextureFilename,
                meshData.roughnessTextureFilename,
                newMesh->metallicRoughnessTexture,
                newMesh->metallicRoughnessSRV
            );
        }

        if (!meshData.metallicTextureFilename.empty()) 
        {
            m_basicPixelConstData.useMetallicMap = true;
        }

        if (!meshData.roughnessTextureFilename.empty()) 
        {
            m_basicPixelConstData.useRoughnessMap = true;
        }

        // 공용 상수 버퍼를 사용 중, 나중엔 메쉬별로 분리
        newMesh->vertexConstantBuffer = m_vertexConstantBuffer;
        newMesh->pixelConstantBuffer = m_pixelConstantBuffer;

        this->m_meshes.push_back(newMesh);
    }

	// InputLayout 설정
    vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = 
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

	// 셰이더 컴파일
    D3D::Get()->CreateVertexShaderAndInputLayout(L"ObjectShader/ObjectVertexShader.hlsl", basicInputElements, m_vertexShader, m_inputLayout);
    D3D::Get()->CreatePixelShader(L"ObjectShader/ObjectPixelShader.hlsl", m_pixelShader);

	// normal 정보 설정
    NormalInit(meshes, basicInputElements);
}

void MeshGroup::Destroy()
{

}

void MeshGroup::UpdateGUI()
{
    ImGui::Begin(("Model" + Name).c_str());
    {
        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("Texture"))
        {
            ImGui::Checkbox("UseTexture", &m_basicPixelConstData.UseTexture);

            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("Material"))
        {
            // 값이 변경되면 flag값이 변경되고 업데이트 하면 됨. 지금은 아무것도 안함
            int flag = 0;

            flag += ImGui::SliderFloat("Metallic", &m_basicPixelConstData.material.metallic, 0.0f, 1.0f);
            flag += ImGui::SliderFloat("Roughness", &m_basicPixelConstData.material.roughness,
                0.0f, 1.0f);
            flag += ImGui::CheckboxFlags("AlbedoTexture", &m_basicPixelConstData.useAlbedoMap, 1);
            flag += ImGui::CheckboxFlags("EmissiveTexture", &m_basicPixelConstData.useEmissiveMap,1);
            flag += ImGui::CheckboxFlags("Use NormalMapping", &m_basicPixelConstData.useNormalMap,1);
            flag += ImGui::CheckboxFlags("Use AO", &m_basicPixelConstData.useAOMap, 1);
            flag += ImGui::CheckboxFlags("Use HeightMapping", &m_basicVertexConstData.useHeightMap,1);
            flag += ImGui::SliderFloat("HeightScale", &m_basicVertexConstData.heightScale, 0.0f,0.1f);
            flag += ImGui::CheckboxFlags("Use MetallicMap", &m_basicPixelConstData.useMetallicMap,1);
            flag += ImGui::CheckboxFlags("Use RoughnessMap",&m_basicPixelConstData.useRoughnessMap, 1);
            flag += ImGui::Checkbox("Draw Normals", &m_drawNormals);

            ImGui::TreePop();
        }
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

void MeshGroup::Tick()
{
    // ModelTransform(VS)
    m_basicVertexConstData.modelWorld = transform->GetWorldMatrix().Transpose();

    m_basicVertexConstData.invTranspose = m_basicVertexConstData.modelWorld;
    m_basicVertexConstData.invTranspose.Translation(Vector3(0.0f));
    m_basicVertexConstData.invTranspose = m_basicVertexConstData.invTranspose.Transpose().Invert();

    // Eye(PS)
    m_basicPixelConstData.EyeWorld = CContext::Get()->GetCamera()->GetPosition();

    // Normal(VS)(b0)
    worldInvConstantBufferData.World = transform->GetWorldMatrix().Transpose();
    worldInvConstantBufferData.InvTranspose = worldInvConstantBufferData.World;
    worldInvConstantBufferData.InvTranspose.Translation(Vector3(0.0f));
    worldInvConstantBufferData.InvTranspose = worldInvConstantBufferData.InvTranspose.Transpose().Invert();

    // Light
    Vector3 curPos = transform->GetPosition();
    Engine::Get()->GetLightManager()->UpdateCBuffer(lightConstantBufferData, curPos, MAX_LIGHTS);


    // BufferData Update
    D3D::Get()->UpdateBuffer(m_basicVertexConstData, m_vertexConstantBuffer);
    D3D::Get()->UpdateBuffer(m_basicPixelConstData, m_pixelConstantBuffer);
    D3D::Get()->UpdateBuffer(worldInvConstantBufferData, worldInvConstantBuffer);
    D3D::Get()->UpdateBuffer(lightConstantBufferData, lightConstantBuffer);

    // 변경되었을 때만 GPU 업데이트
    if (m_drawNormals && m_drawNormalsDirtyFlag)
    {
        D3D::Get()->UpdateBuffer(normalVertexConstantBufferData, normalVertexConstantBuffer);
        m_drawNormalsDirtyFlag = false;
    }
}

void MeshGroup::Render()
{
    UINT stride = sizeof(FVertexPNTT);
    UINT offset = 0;
    ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();

    for (const auto& mesh : m_meshes) 
    {
        // VS
        context->VSSetShader(m_vertexShader.Get(), 0, 0);
        context->VSSetShaderResources(0, 1, mesh->heightSRV.GetAddressOf());
        context->VSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());
        context->VSSetConstantBuffers(0, 1, mesh->vertexConstantBuffer.GetAddressOf());

        // PS
        vector<ID3D11SamplerState*> samplers = 
        {
            GraphicsDevice::Get()->GetLinearWrapSampler(),
            GraphicsDevice::Get()->GetLinearClampSampler() 
        };
        context->PSSetSamplers(0, UINT(samplers.size()), samplers.data());
        context->PSSetShader(m_pixelShader.Get(), 0, 0);

        // 물체 렌더링할 때 여러가지 텍스춰 사용
        ID3D11ShaderResourceView* IBLspecular = Engine::Get()->GetTextureManager()->Get(L"SkyBox_Specular");
        ID3D11ShaderResourceView* IBLIrradiance = Engine::Get()->GetTextureManager()->Get(L"SkyBox_Diffuse");
        ID3D11ShaderResourceView* IBLBRDF = Engine::Get()->GetTextureManager()->Get(L"SkyBox_BRDF");

        vector<ID3D11ShaderResourceView*> resViews = 
        {
            IBLspecular,
            IBLIrradiance,
            IBLBRDF,
            mesh->albedoSRV.Get(),
            mesh->normalSRV.Get(),
            mesh->aoSRV.Get(),
            mesh->metallicRoughnessSRV.Get(),
            mesh->emissiveSRV.Get() 
        };
        context->PSSetShaderResources(0, UINT(resViews.size()), resViews.data());
        context->PSSetConstantBuffers(0, 1,mesh->pixelConstantBuffer.GetAddressOf());
        context->PSSetConstantBuffers(2, 1, lightConstantBuffer.GetAddressOf());

        // IA
        context->IASetInputLayout(m_inputLayout.Get());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->DrawIndexed(mesh->m_indexCount, 0, 0);

        // Normal
        if (m_drawNormals) 
        {
            // VS
            context->VSSetShader(m_normalVertexShader.Get(), 0, 0);
            context->VSSetConstantBuffers(0, 1, worldInvConstantBuffer.GetAddressOf());

            // GS
            context->GSSetConstantBuffers(0, 1, worldInvConstantBuffer.GetAddressOf());
            context->GSSetShader(m_normalGeometryShdaer.Get(), 0, 0);

            // PS
            context->PSSetShader(m_normalPixelShader.Get(), 0, 0);

            // IA
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
            context->Draw(mesh->m_vertexCount, 0);
            context->GSSetShader(NULL, 0, 0);
        }
    }
}

void MeshGroup::NormalInit(vector<PBRMeshData>& meshes, vector<D3D11_INPUT_ELEMENT_DESC>& basicInputElements)
{
	// Shader 컴파일
	D3D::Get()->CreateVertexShaderAndInputLayout(L"NormalVertexShader.hlsl", basicInputElements, m_normalVertexShader, m_inputLayoutNormal);
	D3D::Get()->CreateGeometryShader(L"NormalGeometryShader.hlsl", m_normalGeometryShdaer);
	D3D::Get()->CreatePixelShader(L"NormalPixelShader.hlsl", m_normalPixelShader);
    D3D::Get()->CreateConstantBuffer(normalVertexConstantBufferData, normalVertexConstantBuffer);
}
