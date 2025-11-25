#include "Framework.h"
#include "PostEffects.h"

void PostEffects::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	PBRMeshData meshData =  GeometryGenerator::MakeSquare();

	mesh = make_shared<Mesh>();
	D3D11Utils::CreateVertexBuffer(device, meshData.vertices, mesh->vertexBuffer);

	mesh->indexCount = UINT(meshData.indices.size());
	D3D11Utils::CreateIndexBuffer(device, meshData.indices, mesh->indexBuffer);
}

void PostEffects::Render(ComPtr<ID3D11DeviceContext>& context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(mesh->indexCount,0,0);
}