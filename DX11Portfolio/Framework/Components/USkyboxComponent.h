#pragma once

class USkyboxComponent : public UPrimitiveComponent
{
public:
	void Init() override;
	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context) override;

public:
	ERenderPass GetRenderPass() const override { return ERenderPass::Skybox; }

public:
	void Render(ComPtr<ID3D11DeviceContext>& context) override;
	void RenderNormal(ComPtr<ID3D11DeviceContext>& context) override;
};