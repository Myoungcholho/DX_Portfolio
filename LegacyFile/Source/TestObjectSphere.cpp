#include "Pch.h"
#include "TestObjectSphere.h"

auto TestObjectSphere::MakeSphere(const float radius, const int numSlices, const int numStacks)
{
	const float dTheta = -XM_2PI / float(numSlices);
	const float dPhi = -XM_PI / float(numStacks);

	vector<FVertexPNCU> vertices;
	vector<uint16_t> indices;

	for (int i = 0; i <= numStacks; ++i)
	{
		Vector3 StartPoint = Vector3::Transform(Vector3(0.0f, -radius, 0.0f), Matrix::CreateRotationZ(dPhi * i));

		for (int j = 0; j <= numSlices; ++j)
		{
			FVertexPNCU v;

			v.position = Vector3::Transform(StartPoint, Matrix::CreateRotationY(dTheta * float(j)));
			v.normal = v.position;
			v.normal.Normalize();
			v.uv = Vector2(float(j) / numSlices, 1.0f - float(i) / numStacks);

			vertices.push_back(v);
		}
	}

	for (int i = 0; i < numStacks; ++i)
	{
		const int offset = (numSlices + 1) * i;
		for (int j = 0; j < numSlices; ++j)
		{
			indices.push_back(offset + j);
			indices.push_back(offset + j + numSlices + 1);
			indices.push_back(offset + j + 1 + numSlices + 1);

			indices.push_back(offset + j);
			indices.push_back(offset + j + 1 + numSlices + 1);
			indices.push_back(offset + j + 1);

			//cout << offset + j << " " << offset + j + numSlices + 1 << " " << offset + j + 1 + numSlices + 1 << "\n";
			//cout << offset + j << " " << offset + j + 1 + numSlices + 1 << " " << offset + j + 1 << "\n";
		}
	}

	return make_pair(vertices, indices);
}

void TestObjectSphere::Initialize()
{
	auto result = MakeSphere(1, 40, 40);
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

	texture = new CTexture(L"moon.png");
}

void TestObjectSphere::Destroy()
{
}

void TestObjectSphere::UpdateGUI()
{
}

void TestObjectSphere::Tick()
{
	ConstantBufferData.World =
		Matrix::CreateScale(1.0f) * Matrix::CreateRotationY(0.0f) *
		Matrix::CreateTranslation(Vector3(0.0f, -4.0f, 10.0f));
	ConstantBufferData.World = ConstantBufferData.World.Transpose();

	ConstantBufferData.InvTranspose = ConstantBufferData.World;
	ConstantBufferData.InvTranspose.Translation(Vector3(0.0f));
	ConstantBufferData.InvTranspose = ConstantBufferData.InvTranspose.Transpose().Invert();

	D3D::Get()->UpdateBuffer(ConstantBufferData, Mesh.VSCBuffer);
}

void TestObjectSphere::Render()
{
	D3D::Get()->GetDeviceContext()->VSSetShader(VertexShader.Get(), 0, 0);
	D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(0, 1, Mesh.VSCBuffer.GetAddressOf());
	D3D::Get()->GetDeviceContext()->PSSetShader(PixelShader.Get(), 0, 0);

	// 텍스처 정보
	ID3D11ShaderResourceView* pixelResources[1] = { texture->GetSRV() };
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