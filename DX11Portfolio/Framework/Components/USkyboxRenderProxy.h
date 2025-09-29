#pragma once

class USkyboxRenderProxy : public URenderProxy
{
public:
	void Init(shared_ptr<const CPUMeshAsset> asset) override;

	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context) override;

public:
	void Draw(ID3D11DeviceContext* context) override;
};