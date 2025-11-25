#pragma once

class GraphicsPSO
{
public:
	void operator=(const GraphicsPSO& pso);
	void SetBlendFactor(const float InBlendFactor[4]);
    void Apply(ID3D11DeviceContext* context) const;

public:
    // Shader
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11HullShader> hullShader;
    ComPtr<ID3D11DomainShader> domainShader;
    ComPtr<ID3D11GeometryShader> geometryShader;

    // InputLayout
    ComPtr<ID3D11InputLayout> inputLayout;

    // State
    ComPtr<ID3D11BlendState> blendState;
    ComPtr<ID3D11DepthStencilState> depthStencilState;
    ComPtr<ID3D11RasterizerState> rasterizerState;

    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    UINT stencilRef = 0;

    D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};