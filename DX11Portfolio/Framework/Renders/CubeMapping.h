#pragma once

struct CubeMapping
{
	std::shared_ptr<FMesh> CubeMesh;

	ComPtr<ID3D11ShaderResourceView> CubemapResourceView;

	ComPtr<ID3D11ShaderResourceView> diffuseResView;
	ComPtr<ID3D11ShaderResourceView> specularResView;

	ComPtr<ID3D11VertexShader> VertexShader;
	ComPtr<ID3D11PixelShader> PixelShader;
	ComPtr<ID3D11InputLayout> InputLayout;
};