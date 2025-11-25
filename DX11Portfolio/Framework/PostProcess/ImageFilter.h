#pragma once

class ImageFilter
{
public:
    ImageFilter() {};

    ImageFilter(ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context,
        ComPtr<ID3D11PixelShader>& InPixelShader, int width, int height);

    void Initialize(ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context,
        ComPtr<ID3D11PixelShader>& InPixelShader, int width,
        int height);

    void UpdateConstantBuffers(ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context);

    void Render(ComPtr<ID3D11DeviceContext>& context) const;

    void SetShaderResources(
        const std::vector<ComPtr<ID3D11ShaderResourceView>>& resources);

    void SetRenderTargets(
        const std::vector<ComPtr<ID3D11RenderTargetView>>& targets);

    void OnResize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context,
        int width, int height);


public:
    struct ImageFilterConstData 
    {
        float dx;
        float dy;
        float threshold;
        float strength;
        float option1;
        float option2;
        int filterType;
        float option4;
    };

    ImageFilterConstData constData = {};

protected:
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11Buffer> constBuffer;
    D3D11_VIEWPORT viewport = {};

    // Do not delete pointers
    std::vector<ID3D11ShaderResourceView*> shaderResources;
    std::vector<ID3D11RenderTargetView*> renderTargets;
};