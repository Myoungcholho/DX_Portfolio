#include "Framework.h"
#include "USkinnedMeshRenderProxy.h"

void USkinnedMeshRenderProxy::Init(shared_ptr<const CPUMeshAsset> asset)
{
    meshConstsCPU.World = Matrix();

    D3D11Utils::CreateConstBuffer(device, meshConstsCPU, meshConstsGPU);
    D3D11Utils::CreateConstBuffer(device, materialConstsCPU, materialConstsGPU);

    gpuAsset = GPUAssetManager::LoadGPUMesh(asset);

    for (auto& meshData : asset->meshes)
    {
        if (!meshData.albedoTextureFilename.empty())
            materialConstsCPU.useAlbedoMap = true;

        if (!meshData.emissiveTextureFilename.empty())
            materialConstsCPU.useEmissiveMap = true;

        if (!meshData.normalTextureFilename.empty())
            materialConstsCPU.useNormalMap = true;

        if (!meshData.heightTextureFilename.empty())
            meshConstsCPU.useHeightMap = true;   // 이건 World 관련 상수니까 meshConstsCPU로

        if (!meshData.aoTextureFilename.empty())
            materialConstsCPU.useAOMap = true;

        if (!meshData.metallicTextureFilename.empty())
            materialConstsCPU.useMetallicMap = true;

        if (!meshData.roughnessTextureFilename.empty())
            materialConstsCPU.useRoughnessMap = true;
    }

    // ConstBuffer만 Proxy별로 따로 관리
    /*for (auto& mesh : gpuAsset->meshes)
    {
        mesh->vertexConstBuffer = meshConstsGPU;
        mesh->pixelConstBuffer = materialConstsGPU;
    }*/

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

    for (const auto& mesh : gpuAsset->meshes)
    {
        // Const
        context->VSSetConstantBuffers(0, 1, meshConstsGPU.GetAddressOf());
        context->PSSetConstantBuffers(0, 1, materialConstsGPU.GetAddressOf());

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
    for (const auto& mesh : gpuAsset->meshes)
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

    boneBufferRT.cpuData = initCPU;
    boneBufferRT.Initialize(device.Get());

}

void USkinnedMeshRenderProxy::UpdateBonesGPU()
{
    boneBufferRT.Upload(context.Get());
}

void USkinnedMeshRenderProxy::SetUpdateBones(const vector<Matrix>& bones)
{
    boneBufferRT.cpuData = bones;
}

void USkinnedMeshRenderProxy::SetUpdateBonesCPU(const vector<Matrix>& bones)
{
    bonesPalette = bones;
}