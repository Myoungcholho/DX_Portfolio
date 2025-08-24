#pragma once

class PostEffects
{
public:
    void Initialize
    (
        ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context
    );

    void Render(ComPtr<ID3D11DeviceContext>& context);

private:
    shared_ptr<Mesh> m_mesh;
};