#include "Framework.h"
#include "USkinnedMeshRenderProxy.h"

void USkinnedMeshRenderProxy::Init(const vector<PBRMeshData>& m_meshData)
{
    meshConstsCPU.World = Matrix();

    D3D11Utils::CreateConstBuffer(device, meshConstsCPU, meshConstsGPU);
    D3D11Utils::CreateConstBuffer(device, materialConstsCPU, materialConstsGPU);

    // CPU정보로 GPU데이터, 메시 생성
    for (const auto& meshData : m_meshData)
    {
        auto newMesh = std::make_shared<Mesh>();
        D3D11Utils::CreateVertexBuffer(device, meshData.skinnedVertices, newMesh->vertexBuffer);
        D3D11Utils::CreateIndexBuffer(device, meshData.indices, newMesh->indexBuffer);

        newMesh->indexCount = UINT(meshData.indices.size());
        newMesh->vertexCount = UINT(meshData.skinnedVertices.size());
        newMesh->stride = UINT(sizeof(SkinnedVertex));

        // 텍스쳐 파일이름 있다면 Texture, SRV 만들기
        if (!meshData.albedoTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.albedoTextureFilename, true,
                newMesh->albedoTexture, newMesh->albedoSRV);
            materialConstsCPU.useAlbedoMap = true;
        }

        if (!meshData.emissiveTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.emissiveTextureFilename, true,
                newMesh->emissiveTexture, newMesh->emissiveSRV);
            materialConstsCPU.useEmissiveMap = true;
        }

        if (!meshData.normalTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.normalTextureFilename, false,
                newMesh->normalTexture, newMesh->normalSRV);
            materialConstsCPU.useNormalMap = true;
        }

        if (!meshData.heightTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.heightTextureFilename, false,
                newMesh->heightTexture, newMesh->heightSRV);
            meshConstsCPU.useHeightMap = true;
        }

        if (!meshData.aoTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(device, context,
                meshData.aoTextureFilename, false,
                newMesh->aoTexture, newMesh->aoSRV);
            materialConstsCPU.useAOMap = true;
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
            materialConstsCPU.useMetallicMap = true;
        }

        // 러프니스 이름이 있었다면
        if (!meshData.roughnessTextureFilename.empty())
        {
            materialConstsCPU.useRoughnessMap = true;
        }

        newMesh->vertexConstBuffer = meshConstsGPU;
        newMesh->pixelConstBuffer = materialConstsGPU;

        this->meshes.push_back(newMesh);
    }

}

void USkinnedMeshRenderProxy::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
    D3D11Utils::UpdateBuffer(device, context, meshConstsCPU, meshConstsGPU);
    D3D11Utils::UpdateBuffer(device, context, materialConstsCPU, materialConstsGPU);
}

void USkinnedMeshRenderProxy::Draw(ID3D11DeviceContext* context)
{
    // 여기서 스트럭쳐드 버퍼 업데이트
    UpdateBonesGPU();

    // 본 행렬 SRV 바인딩
    ID3D11ShaderResourceView* srv = boneBufferRT.GetSRV();
    context->VSSetShaderResources(9, 1, &srv);

    for (const auto& mesh : meshes)
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

        // 그리기
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->DrawIndexed(mesh->indexCount, 0, 0);
    }

    // 언바인드
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    context->VSSetShaderResources(9, 1, nullSRV);
}

void USkinnedMeshRenderProxy::DrawNormal(ID3D11DeviceContext* context)
{
    for (const auto& mesh : meshes)
    {
        context->GSSetConstantBuffers(0, 1, meshConstsGPU.GetAddressOf());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);
        context->Draw(mesh->vertexCount, 0);
    }
}

void USkinnedMeshRenderProxy::InitBonesRT(int boneCount)
{
    boneCount = max(1, boneCount);
    vector<Matrix> initCPU(boneCount, Matrix());

    boneBufferRT.m_cpu = initCPU;
    boneBufferRT.Initialize(device.Get());

}

void USkinnedMeshRenderProxy::UpdateBonesGPU()
{
    boneBufferRT.Upload(context.Get());
}

void USkinnedMeshRenderProxy::SetUpdateBones(const vector<Matrix>& bones)
{
    boneBufferRT.m_cpu = bones;
}