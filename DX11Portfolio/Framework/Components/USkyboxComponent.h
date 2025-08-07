#pragma once

class USkyboxComponent : public UPrimitiveComponent
{
public:
	void Init() override;
	void RefreshConstantsCPU() override;
	void OnGUI() override;

public:
	ERenderPass GetRenderPass() const override { return ERenderPass::Skybox; }

public:
	//void Render(ComPtr<ID3D11DeviceContext>& context) override;
	void RenderNormal(ComPtr<ID3D11DeviceContext>& context) override;

public:
	shared_ptr<URenderProxy> GetRenderProxy() override;

private:
	shared_ptr<USkyboxRenderProxy> renderProxy;
};