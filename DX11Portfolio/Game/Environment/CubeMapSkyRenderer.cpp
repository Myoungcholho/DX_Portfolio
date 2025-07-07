#include "Pch.h"
#include "CubeMapSkyRenderer.h"

#include "DirectXTK/DDSTextureLoader.h"

void CubeMapSkyRenderer::Initialize()
{
	CreateCubemapTexture(SkyBoxFilename.c_str(), m_cubeMapping.CubemapResourceView);
	CreateCubemapTexture(SkyBoxDiffuseFilename.c_str(), m_cubeMapping.diffuseResView);
	CreateCubemapTexture(SkyBoxSpecularFilename.c_str(), m_cubeMapping.specularResView);

	TextureManager* textureManager =  Engine::Get()->GetTextureManager();
	textureManager->Register(L"SkyBox", m_cubeMapping.CubemapResourceView.Get());
	textureManager->Register(L"SkyBox_Diffuse", m_cubeMapping.diffuseResView.Get());
	textureManager->Register(L"SkyBox_Specular", m_cubeMapping.specularResView.Get());

	m_cubeMapping.CubeMesh = std::make_shared<FMesh>();

	// VS ConstantBufferData
	WorldMatrixData.World = Matrix();
	WorldMatrixData.InvTranspose = Matrix();
	ViewProjectionData.view = Matrix();
	ViewProjectionData.projection = Matrix();

	// VS의 ConstantBuffer
	D3D::Get()->CreateConstantBuffer(WorldMatrixData, m_cubeMapping.CubeMesh->VSCBuffer);
	D3D::Get()->CreateConstantBuffer(ViewProjectionData, ViewProjectionBuffer);

	MeshData cubeMeshData = GeomtryGenerator::MakeBox(200.0f);
	std::reverse(cubeMeshData.indices.begin(), cubeMeshData.indices.end());

	// IA에 넣을 Vertex/Indices/Count
	D3D::Get()->CreateVertexBuffer(cubeMeshData.vertices, m_cubeMapping.CubeMesh->VertexBuffer);
	m_cubeMapping.CubeMesh->IndexCount = UINT(cubeMeshData.indices.size());
	D3D::Get()->CreateVertexBuffer(cubeMeshData.indices, m_cubeMapping.CubeMesh->IndexBuffer);

	// 셰이더 초기화
	vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	D3D::Get()->CreateVertexShaderAndInputLayout
	(
		L"VS_CubeMapping.hlsl",
		basicInputElements,
		m_cubeMapping.VertexShader,
		m_cubeMapping.InputLayout
	);
	D3D::Get()->CreatePixelShader
	(
		L"PS_CubeMapping.hlsl",
		m_cubeMapping.PixelShader
	);
}

void CubeMapSkyRenderer::Destroy()
{
}

void CubeMapSkyRenderer::UpdateGUI()
{
}

void CubeMapSkyRenderer::Tick()
{
	Matrix view = CContext::Get()->GetViewMatrix();
	view._41 = 0.0f, view._42 = 0.0f, view._43 = 0.0f;
	ViewProjectionData.view = view.Transpose();
	ViewProjectionData.projection = CContext::Get()->GetProjectionMatrix().Transpose();

	D3D::Get()->UpdateBuffer(ViewProjectionData, ViewProjectionBuffer);

	WorldMatrixData.InvTranspose = WorldMatrixData.World;
	WorldMatrixData.InvTranspose.Translation(Vector3(0.0f));
	WorldMatrixData.InvTranspose = WorldMatrixData.InvTranspose.Transpose().Invert();

	D3D::Get()->UpdateBuffer(WorldMatrixData, m_cubeMapping.CubeMesh->VSCBuffer);
}

void CubeMapSkyRenderer::Render()
{
	UINT stride = sizeof(FVertex);
	UINT offset = 0;

	D3D::Get()->GetDeviceContext()->IASetInputLayout(m_cubeMapping.InputLayout.Get());
	D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, m_cubeMapping.CubeMesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	D3D::Get()->GetDeviceContext()->IASetIndexBuffer(m_cubeMapping.CubeMesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::Get()->GetDeviceContext()->VSSetShader(m_cubeMapping.VertexShader.Get(), 0, 0);
	D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(0, 1, m_cubeMapping.CubeMesh->VSCBuffer.GetAddressOf());
	D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(2, 1, ViewProjectionBuffer.GetAddressOf());

	ID3D11ShaderResourceView* views[3] = 
	{ 
		m_cubeMapping.CubemapResourceView.Get(),
		m_cubeMapping.diffuseResView.Get(),
		m_cubeMapping.specularResView.Get()
	};
	D3D::Get()->GetDeviceContext()->PSSetShaderResources(0, 3, views);
	D3D::Get()->GetDeviceContext()->PSSetShader(m_cubeMapping.PixelShader.Get(), 0, 0);
	D3D::Get()->GetDeviceContext()->PSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());

	D3D::Get()->GetDeviceContext()->DrawIndexed(m_cubeMapping.CubeMesh->IndexCount, 0, 0);
}

void CubeMapSkyRenderer::CreateCubemapTexture(const wchar_t* filename, ComPtr<ID3D11ShaderResourceView>& textureResourceView)
{
	ComPtr<ID3D11Texture2D> texture;

	auto hr = CreateDDSTextureFromFileEx
	(
		D3D::Get()->GetDevice(),
		filename,
		0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE, 0,
		D3D11_RESOURCE_MISC_TEXTURECUBE,
		DDS_LOADER_FLAGS(false), (ID3D11Resource**)texture.GetAddressOf(),
		textureResourceView.GetAddressOf(), nullptr
	);

	if (FAILED(hr)) 
		std::cout << "CreateDDSTextureFromFileEx() failed" << std::endl;
	
}