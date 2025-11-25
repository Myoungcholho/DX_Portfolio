#pragma once

#include "Systems/IExecutable.h"

#include <directxtk/SimpleMath.h>
#include <vector>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class Model
{
public:
    Model() {}
    Model(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
        const std::string& basePath, const std::string& filename);
    Model(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
        const std::vector<PBRMeshData>& InMeshes);

    void Initialize(ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context,
        const std::string& basePath, const std::string& filename);

    void Initialize(ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context,
        const std::vector<PBRMeshData>& InMeshes);

    void UpdateConstantBuffers(ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context);

    void Render(ComPtr<ID3D11DeviceContext>& context);

    void RenderNormals(ComPtr<ID3D11DeviceContext>& context);

    void UpdateWorldRow(const Matrix& worldRow);

public:
    Matrix worldRow = Matrix();   // Model(Object) To World За·Д
    Matrix worldITRow = Matrix(); // InverseTranspose

    WorldInvConstantBuffer meshConstsCPU;
    MaterialConstants materialConstsCPU;

    bool drawNormals = false;
    bool isVisible = true;

    std::vector<shared_ptr<Mesh>> meshes;

private:
    ComPtr<ID3D11Buffer> meshConstsGPU;
    ComPtr<ID3D11Buffer> materialConstsGPU;
};