#include "Pch.h"
#include "TestObjectPlane.h"

#include <tuple>
#include <vector>

auto TestObjectPlane::MakePlane()
{
	vector<Vector3> position;
	vector<Vector3> colors;
	vector<Vector3> normals;
	vector<Vector2> uv;

	const float scale = 5.0f;

	position.push_back(Vector3(-1.0, 0.0, 1.0) * scale);
	position.push_back(Vector3(1.0, 0.0, 1.0) * scale);
	position.push_back(Vector3(1.0, 0.0, -1.0) * scale);
	position.push_back(Vector3(-1.0, 0.0, -1.0) * scale);
	colors.push_back(Vector3(1.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	uv.push_back(Vector2(0.0f, 0.0f));
	uv.push_back(Vector2(1.0f, 0.0f));
	uv.push_back(Vector2(1.0f, 1.0f));
	uv.push_back(Vector2(0.0f, 1.0f));

	vector<FVertexPNCU> vertices;
	for (size_t i = 0; i < position.size(); i++)
	{
		FVertexPNCU v;
		v.position = position[i];
		v.color = colors[i];
		v.normal = normals[i];
		v.uv = uv[i];
		vertices.push_back(v);
	}

	vector<uint16_t> indices =
	{
		0,1,2,0,2,3
	};

	return std::make_tuple(vertices, indices);
}

void TestObjectPlane::Initialize()
{
	auto result = MakePlane();
	auto& vertices = std::get<0>(result);
	auto& indices = std::get<1>(result);

	D3D::Get()->CreateVertexBuffer(vertices, VertexBuffer);
	IndexCount = UINT(indices.size());
	D3D::Get()->CreateIndexBuffer(indices, IndexBuffer);

	ConstantBufferData.World = Matrix();
	ConstantBufferData.InvTranspose = Matrix();

	D3D::Get()->CreateConstantBuffer(ConstantBufferData, ConstantBuffer);

	vector<D3D11_INPUT_ELEMENT_DESC> inputElements =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36 , D3D11_INPUT_PER_VERTEX_DATA, 0}, // 8
	};

	D3D::Get()->CreateVertexShaderAndInputLayout(L"FloorVS.hlsl", inputElements,
		VertexShader, InputLayout);
	D3D::Get()->CreatePixelShader(L"FloorPS.hlsl", PixelShader);

	// 텍스처
	//texture = new CTexture(L"wall.jpg");
	texture = new CTexture(L"cat.png");
}

void TestObjectPlane::Destroy()
{
}

void TestObjectPlane::UpdateGUI()
{
}

void TestObjectPlane::Tick()
{
	constexpr float angleRad = DirectX::XMConvertToRadians(-90.0f);

	ConstantBufferData.World =
		Matrix::CreateScale(1.0f) * Matrix::CreateRotationX(angleRad) *
		Matrix::CreateTranslation(Vector3(0.0f, -3.0f, 20.0f));
	ConstantBufferData.World = ConstantBufferData.World.Transpose();
	
	ConstantBufferData.InvTranspose = ConstantBufferData.World;
	ConstantBufferData.InvTranspose.Translation(Vector3(0.0f));
	ConstantBufferData.InvTranspose = ConstantBufferData.InvTranspose.Transpose().Invert();

	D3D::Get()->UpdateBuffer(ConstantBufferData, ConstantBuffer);
}

void TestObjectPlane::Render()
{
	D3D::Get()->GetDeviceContext()->VSSetShader(VertexShader.Get(), 0, 0);
	D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	D3D::Get()->GetDeviceContext()->PSSetShader(PixelShader.Get(), 0, 0);

	// 텍스처 정보
	ID3D11ShaderResourceView* pixelResources[1] = { texture->GetSRV() };
	D3D::Get()->GetDeviceContext()->PSSetShaderResources(0, 1, pixelResources);
	D3D::Get()->GetDeviceContext()->PSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());


	UINT stride = sizeof(FVertexPNCU);
	UINT offset = 0;
	D3D::Get()->GetDeviceContext()->IASetInputLayout(InputLayout.Get());
	D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
	D3D::Get()->GetDeviceContext()->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::Get()->GetDeviceContext()->DrawIndexed(IndexCount, 0, 0);

}