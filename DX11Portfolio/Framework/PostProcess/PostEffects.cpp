#include "Framework.h"
#include "PostEffects.h"

void PostEffects::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	PBRMeshData meshData =  GeomtryGenerator::MakeSquare();

	m_mesh = make_shared<Mesh>();
	D3D11Utils::CreateVertexBuffer(device, meshData.vertices, m_mesh->vertexBuffer);

	m_mesh->indexCount = UINT(meshData.indices.size());
	D3D11Utils::CreateIndexBuffer(device, meshData.indices, m_mesh->indexBuffer);
}

void PostEffects::Render(ComPtr<ID3D11DeviceContext>& context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, m_mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(m_mesh->indexCount,0,0);
}