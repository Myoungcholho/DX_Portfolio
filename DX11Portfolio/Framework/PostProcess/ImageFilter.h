#pragma once

class ImageFilter
{
public:
    ImageFilter() {};
    ImageFilter(ComPtr<ID3D11PixelShader>& pixelShader, int width, int height);

public:
    void Initialize(ComPtr<ID3D11PixelShader>& pixelShader, int width,int height);
    void UpdateConstantBuffers();
    void Render() const;

    void SetShaderResources(const std::vector<ComPtr<ID3D11ShaderResourceView>>& resources);
    void SetRenderTargets(const std::vector<ComPtr<ID3D11RenderTargetView>>& targets);

public:
    struct ImageFilterConstData 
    {
        float dx;
        float dy;
        float threshold;
        float strength;
        float option1;
        float option2;
        float option3;
        float option4;
    };

    ImageFilterConstData m_constData = {};

protected:
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11Buffer> m_constBuffer;
    D3D11_VIEWPORT m_viewport = {};

    // 
    std::vector<ID3D11ShaderResourceView*> m_shaderResources;
    std::vector<ID3D11RenderTargetView*> m_renderTargets;
};