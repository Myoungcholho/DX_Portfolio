#include "Framework.h"
#include "USkyboxRenderProxy.h"

void USkyboxRenderProxy::Init(const vector<PBRMeshData>& m_meshData)
{
	m_meshConstsCPU.World = Matrix();

	D3D11Utils::CreateConstBuffer(device, m_meshConstsCPU, m_meshConstsGPU);
	D3D11Utils::CreateConstBuffer(device, m_materialConstsCPU, m_materialConstsGPU);

    // CPU정보로 GPU데이터 생성
    for (const auto& meshData : m_meshData)
    {
        auto newMesh = std::make_shared<Mesh>();
        D3D11Utils::CreateVertexBuffer(device, meshData.vertices, newMesh->vertexBuffer);
        D3D11Utils::CreateIndexBuffer(device, meshData.indices, newMesh->indexBuffer);

        newMesh->indexCount = UINT(meshData.indices.size());
        newMesh->vertexCount = UINT(meshData.vertices.size());
        newMesh->stride = UINT(sizeof(Vertex));

        // 텍스쳐 파일이름 있다면 Texture, SRV 만들기
        if (!meshData.albedoTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.albedoTextureFilename, true,
                newMesh->albedoTexture, newMesh->albedoSRV);
            m_materialConstsCPU.useAlbedoMap = true;
        }

        if (!meshData.emissiveTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.emissiveTextureFilename, true,
                newMesh->emissiveTexture, newMesh->emissiveSRV);
            m_materialConstsCPU.useEmissiveMap = true;
        }

        if (!meshData.normalTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.normalTextureFilename, false,
                newMesh->normalTexture, newMesh->normalSRV);
            m_materialConstsCPU.useNormalMap = true;
        }

        if (!meshData.heightTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.heightTextureFilename, false,
                newMesh->heightTexture, newMesh->heightSRV);
            m_meshConstsCPU.useHeightMap = true;
        }

        if (!meshData.aoTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(device, context,
                meshData.aoTextureFilename, false,
                newMesh->aoTexture, newMesh->aoSRV);
            m_materialConstsCPU.useAOMap = true;
        }

        // metalic과 Roughness
        // Green : Roughness, Blue : Metallic(Metalness)
        if (!meshData.metallicTextureFilename.empty() ||
            !meshData.roughnessTextureFilename.empty())
        {
            D3D11Utils::CreateMetallicRoughnessTexture(
                device, context, meshData.metallicTextureFilename,
                meshData.roughnessTextureFilename,
                newMesh->metallicRoughnessTexture,
                newMesh->metallicRoughnessSRV);
        }

        // 메탈릭 이름이 있었다면
        if (!meshData.metallicTextureFilename.empty())
        {
            m_materialConstsCPU.useMetallicMap = true;
        }

        // 러프니스 이름이 있었다면
        if (!meshData.roughnessTextureFilename.empty())
        {
            m_materialConstsCPU.useRoughnessMap = true;
        }

        // 공용으로 사용,음.. 괜찮나 괜찮은거 같기도 하고
        newMesh->vertexConstBuffer = m_meshConstsGPU;
        newMesh->pixelConstBuffer = m_materialConstsGPU;

        this->m_meshes.push_back(newMesh);
    }
}

void USkyboxRenderProxy::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
    // m_meshConstsCpu의 값 업데이트는 CPU에서 Set으로 밀어주고있다.

    D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU, m_meshConstsGPU);
    D3D11Utils::UpdateBuffer(device, context, m_materialConstsCPU, m_materialConstsGPU);
}

void USkyboxRenderProxy::Draw(ID3D11DeviceContext* context)
{
    for (const auto& mesh : m_meshes)
    {
        // Const
        context->VSSetConstantBuffers(0, 1, mesh->vertexConstBuffer.GetAddressOf());
        context->PSSetConstantBuffers(0, 1, mesh->pixelConstBuffer.GetAddressOf());

        // SRV
        // 물체 렌더링할 때 여러가지 텍스춰 사용 (t0 부터시작)
        vector<ID3D11ShaderResourceView*> resViews =
        {
            mesh->albedoSRV.Get(),
            mesh->normalSRV.Get(),
            mesh->aoSRV.Get(),
            mesh->metallicRoughnessSRV.Get(),
            mesh->emissiveSRV.Get()
        };
        context->PSSetShaderResources(0, UINT(resViews.size()), resViews.data());
        context->VSSetShaderResources(0, 1, mesh->heightSRV.GetAddressOf());

        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->DrawIndexed(mesh->indexCount, 0, 0);
    }
}