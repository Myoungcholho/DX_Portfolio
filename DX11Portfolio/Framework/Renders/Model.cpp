#include "Framework.h"
#include "Model.h"


Model::Model(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
    const std::string& basePath, const std::string& filename) {
    this->Initialize(device, context, basePath, filename);
}

Model::Model(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
    const std::vector<PBRMeshData>& InMeshes) {
    this->Initialize(device, context, InMeshes);
}

void Model::Initialize(ComPtr<ID3D11Device>& device,
    ComPtr<ID3D11DeviceContext>& context,
    const std::string& basePath,
    const std::string& filename) {

    auto meshes = GeometryGenerator::ReadFromFile(basePath, filename);

    Initialize(device, context, meshes);
}

void Model::Initialize(ComPtr<ID3D11Device>& device,
    ComPtr<ID3D11DeviceContext>& context,
    const std::vector<PBRMeshData>& InMeshes)
{

    // ConstantBuffer 만들기
    meshConstsCPU.World = Matrix();

    D3D11Utils::CreateConstBuffer(device, meshConstsCPU, meshConstsGPU);
    D3D11Utils::CreateConstBuffer(device, materialConstsCPU,
        materialConstsGPU);

    for (const auto& meshData : InMeshes)
    {
        auto newMesh = std::make_shared<Mesh>();
        D3D11Utils::CreateVertexBuffer(device, meshData.vertices,
            newMesh->vertexBuffer);
        newMesh->indexCount = UINT(meshData.indices.size());
        newMesh->vertexCount = UINT(meshData.vertices.size());
        newMesh->stride = UINT(sizeof(Vertex));
        D3D11Utils::CreateIndexBuffer(device, meshData.indices,
            newMesh->indexBuffer);

        if (!meshData.albedoTextureFilename.empty()) {
            D3D11Utils::CreateTexture(
                device, context, meshData.albedoTextureFilename, true,
                newMesh->albedoTexture, newMesh->albedoSRV);
            materialConstsCPU.useAlbedoMap = true;
        }

        if (!meshData.emissiveTextureFilename.empty()) {
            D3D11Utils::CreateTexture(
                device, context, meshData.emissiveTextureFilename, true,
                newMesh->emissiveTexture, newMesh->emissiveSRV);
            materialConstsCPU.useEmissiveMap = true;
        }

        if (!meshData.normalTextureFilename.empty()) {
            D3D11Utils::CreateTexture(
                device, context, meshData.normalTextureFilename, false,
                newMesh->normalTexture, newMesh->normalSRV);
            materialConstsCPU.useNormalMap = true;
        }

        if (!meshData.heightTextureFilename.empty()) {
            D3D11Utils::CreateTexture(
                device, context, meshData.heightTextureFilename, false,
                newMesh->heightTexture, newMesh->heightSRV);
            meshConstsCPU.useHeightMap = true;
        }

        if (!meshData.aoTextureFilename.empty()) {
            D3D11Utils::CreateTexture(device, context,
                meshData.aoTextureFilename, false,
                newMesh->aoTexture, newMesh->aoSRV);
            materialConstsCPU.useAOMap = true;
        }

        // GLTF 방식으로 Metallic과 Roughness를 한 텍스춰에 넣음
        // Green : Roughness, Blue : Metallic(Metalness)
        if (!meshData.metallicTextureFilename.empty() ||
            !meshData.roughnessTextureFilename.empty()) {
            D3D11Utils::CreateMetallicRoughnessTexture(
                device, context, meshData.metallicTextureFilename,
                meshData.roughnessTextureFilename,
                newMesh->metallicRoughnessTexture,
                newMesh->metallicRoughnessSRV);
        }

        if (!meshData.metallicTextureFilename.empty()) {
            materialConstsCPU.useMetallicMap = true;
        }

        if (!meshData.roughnessTextureFilename.empty()) {
            materialConstsCPU.useRoughnessMap = true;
        }

        this->meshes.push_back(newMesh);
    }
}

void Model::UpdateConstantBuffers(ComPtr<ID3D11Device>& device,
    ComPtr<ID3D11DeviceContext>& context) {
    if (isVisible) {
        D3D11Utils::UpdateBuffer(device, context, meshConstsCPU,
            meshConstsGPU);

        D3D11Utils::UpdateBuffer(device, context, materialConstsCPU,
            materialConstsGPU);
    }
}

void Model::Render(ComPtr<ID3D11DeviceContext>& context) {
    if (isVisible) {
        for (const auto& mesh : meshes) {
            /*context->VSSetConstantBuffers(
                0, 1, mesh->vertexConstBuffer.GetAddressOf());
            context->PSSetConstantBuffers(
                0, 1, mesh->pixelConstBuffer.GetAddressOf());*/

            context->VSSetShaderResources(0, 1, mesh->heightSRV.GetAddressOf());

            // 물체 렌더링할 때 여러가지 텍스춰 사용 (t0 부터시작)
            vector<ID3D11ShaderResourceView*> resViews = {
                mesh->albedoSRV.Get(), mesh->normalSRV.Get(), mesh->aoSRV.Get(),
                mesh->metallicRoughnessSRV.Get(), mesh->emissiveSRV.Get() };
            context->PSSetShaderResources(0, UINT(resViews.size()),
                resViews.data());

            context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
                &mesh->stride, &mesh->offset);

            context->IASetIndexBuffer(mesh->indexBuffer.Get(),
                DXGI_FORMAT_R32_UINT, 0);
            context->DrawIndexed(mesh->indexCount, 0, 0);
        }
    }
}

void Model::RenderNormals(ComPtr<ID3D11DeviceContext>& context) {
    for (const auto& mesh : meshes) {
        context->GSSetConstantBuffers(0, 1, meshConstsGPU.GetAddressOf());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
            &mesh->stride, &mesh->offset);
        context->Draw(mesh->vertexCount, 0);
    }
}

void Model::UpdateWorldRow(const Matrix& worldRow) {
    this->worldRow = worldRow;
    this->worldITRow = worldRow;
    worldITRow.Translation(Vector3(0.0f));
    worldITRow = worldITRow.Invert().Transpose();

    meshConstsCPU.World = worldRow.Transpose();
    meshConstsCPU.InvTranspose = worldITRow.Transpose();
}