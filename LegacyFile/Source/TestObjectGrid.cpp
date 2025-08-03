#include "Pch.h"
#include "TestObjectGrid.h"

auto TestObjectGrid::MakeGrid(const float width, const float height, const int numSlices, const int numStacks)
{
	const float dx = width / numSlices;
	const float dy = height / numStacks;

	vector<FVertexPNCU> vertices;
	vector<uint16_t> indices;

	Vector3 stackStartPoint = Vector3(width * -0.5f, 0.0f, height * -0.5f);
	for (int i = 0; i <= numStacks; ++i)
	{
		for (int j = 0; j <= numSlices; ++j)
		{
			FVertexPNCU v;

			float x = dx * j;
			float z = dy * i;
			float y = 0.0f;

			v.position = Vector3(x,y,z) + stackStartPoint;
			v.normal = Vector3(0.f, 1.f, 0.0f);
			v.color = Vector3(1.0f, 1.0f, 1.0f);
			v.uv = Vector2(float(j)/numSlices , 1-float(i)/numStacks);
			vertices.push_back(v);
		}
	}

	for (int i = 0; i < numStacks; ++i)
	{
		for (int j = 0; j < numSlices; ++j)
		{
			indices.push_back(i * (numSlices + 1) + j);
			indices.push_back((i + 1) * (numSlices + 1) + j + 1);
			indices.push_back(i * (numSlices + 1) + j + 1);

			indices.push_back(i * (numSlices + 1) + j);
			indices.push_back((i + 1) * (numSlices + 1) + j);
			indices.push_back((i + 1) * (numSlices + 1) + j + 1);
		}
	}

	return std::make_tuple(vertices, indices);
}

void TestObjectGrid::Initialize()
{
	auto result = MakeGrid(15,15,5,5);
	vector<FVertexPNCU>& vertices = get<0>(result);
	vector<uint16_t>& indices = get<1>(result);

	D3D::Get()->CreateVertexBuffer(vertices, Mesh.VertexBuffer);
	Mesh.IndexCount = UINT(indices.size());
	D3D::Get()->CreateIndexBuffer(indices, Mesh.IndexBuffer);

	ConstantBufferData.World = Matrix();
	ConstantBufferData.InvTranspose = Matrix();

	D3D::Get()->CreateConstantBuffer(ConstantBufferData, Mesh.VSCBuffer);

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

	// Texture
	//texture = new CTexture(L"cat2.png");
	D3D::Get()->CreateTexture("../../../_Textures/cat2.png",true,texture,textureSRV);
}

void TestObjectGrid::Destroy()
{

}

void TestObjectGrid::UpdateGUI()
{

}

void TestObjectGrid::Tick()
{
	ConstantBufferData.World =
		Matrix::CreateScale(1.0f) * Matrix::CreateRotationY(0.0f) *
		Matrix::CreateTranslation(Vector3(0.0f, -5.0f, 10.0f));
	ConstantBufferData.World = ConstantBufferData.World.Transpose();
	
	ConstantBufferData.InvTranspose = ConstantBufferData.World;
	ConstantBufferData.InvTranspose.Translation(Vector3(0.0f));
	ConstantBufferData.InvTranspose = ConstantBufferData.InvTranspose.Transpose().Invert();

	D3D::Get()->UpdateBuffer(ConstantBufferData, Mesh.VSCBuffer);
}

void TestObjectGrid::Render()
{
	D3D::Get()->GetDeviceContext()->VSSetShader(VertexShader.Get(), 0, 0);
	D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(0, 1, Mesh.VSCBuffer.GetAddressOf());
	D3D::Get()->GetDeviceContext()->PSSetShader(PixelShader.Get(), 0, 0);

	// 텍스처 정보
	//ID3D11ShaderResourceView* pixelResources[1] = { texture->GetSRV()};
	ID3D11ShaderResourceView* pixelResources[1] = { textureSRV.Get() };
	D3D::Get()->GetDeviceContext()->PSSetShaderResources(0, 1, pixelResources);
	D3D::Get()->GetDeviceContext()->PSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());


	UINT stride = sizeof(FVertexPNCU);
	UINT offset = 0;
	D3D::Get()->GetDeviceContext()->IASetInputLayout(InputLayout.Get());
	D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, Mesh.VertexBuffer.GetAddressOf(), &stride, &offset);
	D3D::Get()->GetDeviceContext()->IASetIndexBuffer(Mesh.IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::Get()->GetDeviceContext()->DrawIndexed(Mesh.IndexCount, 0, 0);
}
