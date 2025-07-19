#pragma once

struct CubeMapping
{
	std::shared_ptr<FMesh> CubeMesh;

	ComPtr<ID3D11ShaderResourceView> envSRV;
	ComPtr<ID3D11ShaderResourceView> irradianceSRV;
	ComPtr<ID3D11ShaderResourceView> specularSRV;
	ComPtr<ID3D11ShaderResourceView> brdfSRV;

	ComPtr<ID3D11VertexShader> VertexShader;
	ComPtr<ID3D11PixelShader> PixelShader;
	ComPtr<ID3D11InputLayout> InputLayout;
};