#pragma once

#include "BillboardPoint.h"
#include <vector>
#include <wrl/client.h>
#include <d3d11.h>

class BillboardInstance
{
public:
    void Initialize(vector<BillboardPoint>& points);
    void Tick();
    void UpdateGUI();
    void Render();

public:
    const std::vector<BillboardPoint>& GetPoints() const;

    ComPtr<ID3D11Buffer> GetVertexBuffer();
    size_t GetPointCount();

public:
    shared_ptr<Transform> transform;

private:
    // VS , GS ¿ë
    WorldCBuffer WorldMatrixData;
    ComPtr<ID3D11Buffer> WorldConstantBuffer;

    TreeConstants TreeIndexData;
    ComPtr<ID3D11Buffer> TreeIndexBuffer;

private:
    vector<BillboardPoint> m_points;
    ComPtr<ID3D11Buffer> m_vertexBuffer;
};