#include "Framework.h"
#include "UStaticMeshRenderProxy.h"

void UStaticMeshRenderProxy::Init(shared_ptr<const CPUMeshAsset> asset)
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

void UStaticMeshRenderProxy::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
    D3D11Utils::UpdateBuffer(device, context, meshConstsCPU, meshConstsGPU);
    D3D11Utils::UpdateBuffer(device, context ,materialConstsCPU, materialConstsGPU);
}

void UStaticMeshRenderProxy::Draw(ID3D11DeviceContext* context)
{
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

        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->DrawIndexed(mesh->indexCount, 0, 0);
    }
}

void UStaticMeshRenderProxy::DrawNormal(ID3D11DeviceContext* context)
{
    for (const auto& mesh : gpuAsset->meshes)
    {
        context->GSSetConstantBuffers(0, 1, meshConstsGPU.GetAddressOf());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);
        context->Draw(mesh->vertexCount, 0);
    }
}