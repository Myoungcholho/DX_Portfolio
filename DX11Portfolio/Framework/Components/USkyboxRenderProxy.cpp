#include "Framework.h"
#include "USkyboxRenderProxy.h"

void USkyboxRenderProxy::Init(shared_ptr<const CPUMeshAsset> asset)
{
	meshConstsCPU.World = Matrix();

	D3D11Utils::CreateConstBuffer(device, meshConstsCPU, meshConstsGPU);
	D3D11Utils::CreateConstBuffer(device, materialConstsCPU, materialConstsGPU);

    gpuAsset = GPUAssetManager::LoadGPUMesh(asset);

    /*for (auto& mesh : gpuAsset->meshes)
    {
        mesh->vertexConstBuffer = meshConstsGPU;
        mesh->pixelConstBuffer = materialConstsGPU;
    }*/
}

void USkyboxRenderProxy::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
    // meshConstsCpu의 값 업데이트는 CPU에서 Set으로 밀어주고있다.

    D3D11Utils::UpdateBuffer(device, context, meshConstsCPU, meshConstsGPU);
    D3D11Utils::UpdateBuffer(device, context, materialConstsCPU, materialConstsGPU);
}

void USkyboxRenderProxy::Draw(ID3D11DeviceContext* context)
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