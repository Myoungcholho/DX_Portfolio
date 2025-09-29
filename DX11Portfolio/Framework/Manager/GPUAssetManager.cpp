#include "Framework.h"
#include "GPUAssetManager.h"

unordered_map<const CPUMeshAsset*, weak_ptr<const GPUMeshAsset>> GPUAssetManager::cache;

shared_ptr<const GPUMeshAsset> GPUAssetManager::LoadGPUMesh(shared_ptr<const CPUMeshAsset> cpuAsset)
{
    auto it = cache.find(cpuAsset.get());
    if (it != cache.end())
    {
        if (shared_ptr<const GPUMeshAsset> sp = it->second.lock())          // weak -> shared_ptr 승격
            return sp;
    }

    shared_ptr<const GPUMeshAsset> gpuAsset = BuildGPUMesh_(cpuAsset);
    
    cache.emplace(cpuAsset.get(), gpuAsset);
    return gpuAsset;
}

void GPUAssetManager::ClearUnused()
{
    for (auto it = cache.begin(); it != cache.end();)
    {
        if (it->second.expired())
            it = cache.erase(it);
        else
            ++it;
    }
}

shared_ptr<const GPUMeshAsset> GPUAssetManager::BuildGPUMesh_(shared_ptr<const CPUMeshAsset> cpuAsset)
{
    shared_ptr<GPUMeshAsset> gpuAsset = make_shared<GPUMeshAsset>();

    ComPtr<ID3D11Device> device = D3D::Get()->GetDeviceCom();
    ComPtr<ID3D11DeviceContext> context = D3D::Get()->GetDeviceContextCom();

    for (const PBRMeshData& meshData : cpuAsset->meshes)
    {
        shared_ptr<Mesh> newMesh = make_shared<Mesh>();

        // skinned 메시라면
        if (!meshData.skinnedVertices.empty())
        {
            D3D11Utils::CreateVertexBuffer(device,meshData.skinnedVertices,newMesh->vertexBuffer);
            newMesh->stride = sizeof(SkinnedVertex);
            newMesh->vertexCount = UINT(meshData.skinnedVertices.size());
        }
        else
        {
            D3D11Utils::CreateVertexBuffer(device,meshData.vertices,newMesh->vertexBuffer);
            newMesh->stride = sizeof(Vertex);
            newMesh->vertexCount = UINT(meshData.vertices.size());
        }

        D3D11Utils::CreateIndexBuffer(device, meshData.indices, newMesh->indexBuffer);
        newMesh->indexCount = UINT(meshData.indices.size());

        // 텍스쳐 파일이름 있다면 Texture, SRV 만들기
        if (!meshData.albedoTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.albedoTextureFilename, true,
                newMesh->albedoTexture, newMesh->albedoSRV);
        }

        if (!meshData.emissiveTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.emissiveTextureFilename, true,
                newMesh->emissiveTexture, newMesh->emissiveSRV);
        }

        if (!meshData.normalTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.normalTextureFilename, false,
                newMesh->normalTexture, newMesh->normalSRV);
        }

        if (!meshData.heightTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(
                device, context, meshData.heightTextureFilename, false,
                newMesh->heightTexture, newMesh->heightSRV);
        }

        if (!meshData.aoTextureFilename.empty())
        {
            D3D11Utils::CreateTexture(device, context,
                meshData.aoTextureFilename, false,
                newMesh->aoTexture, newMesh->aoSRV);
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

        gpuAsset->meshes.push_back(newMesh);
    }

    return gpuAsset;
}
