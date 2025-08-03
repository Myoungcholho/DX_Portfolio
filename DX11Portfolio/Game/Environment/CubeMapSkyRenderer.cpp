#include "Pch.h"
#include "CubeMapSkyRenderer.h"

#include "DirectXTK/DDSTextureLoader.h"

void CubeMapSkyRenderer::Initialize()
{
	//transform = make_shared<Transform>();
	//transform->SetPosition(Vector3(0, -50, 0));
	//transform->SetRotation(Vector3(0, 0, 0));

	//// .dds 파일 읽기
	//D3D::Get()->CreateDDSTexture(EnvHDR_Name.c_str(), true, m_cubeMapping.envSRV);
	//D3D::Get()->CreateDDSTexture(Specular_Name.c_str(), true, m_cubeMapping.specularSRV);
	//D3D::Get()->CreateDDSTexture(DiffuseHDR_Name.c_str(), true, m_cubeMapping.irradianceSRV);
	//D3D::Get()->CreateDDSTexture(Brdf_Name.c_str(), false, m_cubeMapping.brdfSRV);

	//// IBL사용하기 위해 저장소에 등록
	//TextureManager* textureManager =  Engine::Get()->GetTextureManager();
	//textureManager->Register(L"SkyBox", m_cubeMapping.envSRV.Get());
	//textureManager->Register(L"SkyBox_Diffuse", m_cubeMapping.irradianceSRV.Get());
	//textureManager->Register(L"SkyBox_Specular", m_cubeMapping.specularSRV.Get());
	//textureManager->Register(L"SkyBox_BRDF", m_cubeMapping.brdfSRV.Get());

	//m_cubeMapping.CubeMesh = std::make_shared<FMesh>();

	//// VS ConstantBufferData
	//worldInvConstantBufferData.World = transform->GetWorldMatrix().Transpose();
	//worldInvConstantBufferData.InvTranspose = transform->GetWorldMatrix().Invert().Transpose();

	//viewProjectionData.viewProj = Matrix();
	//mirrorViewProjectionData.mirrorViewProj = Matrix();

	//// VS의 ConstantBuffer
	//D3D::Get()->CreateConstantBuffer(worldInvConstantBufferData, m_cubeMapping.CubeMesh->VSCBuffer);
	//D3D::Get()->CreateConstantBuffer(viewProjectionData, ViewProjectionBuffer);	// 카메라 기준으로 같이 이동하려고
	//D3D::Get()->CreateConstantBuffer(mirrorViewProjectionData, mirrorViewProjectionBuffer);
	//D3D::Get()->CreateConstantBuffer(cubeMappingConstantBufferData, cubeMappingConstantBuffer);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////PBRMeshData cubeMeshData = GeomtryGenerator::MakeBox(200.0f);
	//PBRMeshData cubeMeshData = GeomtryGenerator::MakeSphere(200.0f,64,64);
	//std::reverse(cubeMeshData.indices.begin(), cubeMeshData.indices.end());

	//// IA에 넣을 Vertex/Indices/Count
	//D3D::Get()->CreateVertexBuffer(cubeMeshData.vertices, m_cubeMapping.CubeMesh->VertexBuffer);

	//m_cubeMapping.CubeMesh->IndexCount = UINT(cubeMeshData.indices.size());
	//D3D::Get()->CreateIndexBuffer(cubeMeshData.indices, m_cubeMapping.CubeMesh->IndexBuffer);

	//// 셰이더 초기화
	//vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = 
	//{
	//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	//	 D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
	//	 D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
	//	 D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	 {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
	//	 D3D11_INPUT_PER_VERTEX_DATA, 0},
	//};

	//D3D::Get()->CreateVertexShaderAndInputLayout
	//(
	//	L"CubeMapping/CubeMappingVS.hlsl",
	//	basicInputElements,
	//	m_cubeMapping.VertexShader,
	//	m_cubeMapping.InputLayout
	//);
	//D3D::Get()->CreatePixelShader(L"CubeMapping/CubeMappingPS.hlsl",m_cubeMapping.PixelShader);
}

void CubeMapSkyRenderer::UpdateGUI()
{
	//ImGui::Begin("SkyCubeMap");
	//{
	//	ImGui::RadioButton("EnV", &cubeMappingConstantBufferData.textureToDraw, 0);
	//	ImGui::RadioButton("Specular", &cubeMappingConstantBufferData.textureToDraw, 1);
	//	ImGui::RadioButton("Irradiance", &cubeMappingConstantBufferData.textureToDraw, 2);
	//	ImGui::SliderFloat("MipLevel", &cubeMappingConstantBufferData.mipLevel, 0.0f, 8.0f);
	//}
	//ImGui::End();
}

void CubeMapSkyRenderer::Tick()
{
	//// PS(b0)
	//D3D::Get()->UpdateBuffer(cubeMappingConstantBufferData, cubeMappingConstantBuffer);

	//// VS(b0)
	//worldInvConstantBufferData.World = transform->GetWorldMatrix().Transpose();
	//worldInvConstantBufferData.InvTranspose = worldInvConstantBufferData.World;
	//worldInvConstantBufferData.InvTranspose.Translation(Vector3(0.0f));
	//worldInvConstantBufferData.InvTranspose = worldInvConstantBufferData.InvTranspose.Transpose().Invert();

	//D3D::Get()->UpdateBuffer(worldInvConstantBufferData, m_cubeMapping.CubeMesh->VSCBuffer);

	//// VS(b2)
	//Matrix view = CContext::Get()->GetViewMatrix();
	//view._41 = 0.0f, view._42 = 0.0f, view._43 = 0.0f;
	//Matrix proj = CContext::Get()->GetProjectionMatrix();
	//Matrix viewProj = view * proj;
	//viewProjectionData.viewProj = viewProj.Transpose();

	//D3D::Get()->UpdateBuffer(viewProjectionData, ViewProjectionBuffer);

	//// mirror
	//Matrix mirror = CContext::Get()->GetMirrorMatrix();
	//mirrorViewProjectionData.mirrorViewProj = (mirror * view * proj).Transpose();

	//D3D::Get()->UpdateBuffer(mirrorViewProjectionData, mirrorViewProjectionBuffer);

}

void CubeMapSkyRenderer::Render(const bool& mirror)
{
	//UINT stride = sizeof(Vertex);
	//UINT offset = 0;

	//// IA
	//D3D::Get()->GetDeviceContext()->IASetInputLayout(m_cubeMapping.InputLayout.Get());
	//D3D::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, m_cubeMapping.CubeMesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	//D3D::Get()->GetDeviceContext()->IASetIndexBuffer(m_cubeMapping.CubeMesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//D3D::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// VS
	//D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(0, 1, m_cubeMapping.CubeMesh->VSCBuffer.GetAddressOf());
	//if(mirror == false)
	//	D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(2, 1, ViewProjectionBuffer.GetAddressOf());
	//else
	//	D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(2, 1, mirrorViewProjectionBuffer.GetAddressOf());
	//D3D::Get()->GetDeviceContext()->VSSetShader(m_cubeMapping.VertexShader.Get(), 0, 0);

	//// PS
	//D3D::Get()->GetDeviceContext()->PSSetShader(m_cubeMapping.PixelShader.Get(), 0, 0);
	//ID3D11ShaderResourceView* views[3] = 
	//{ 
	//	m_cubeMapping.envSRV.Get(),
	//	m_cubeMapping.specularSRV.Get(),
	//	m_cubeMapping.irradianceSRV.Get(),
	//};
	//D3D::Get()->GetDeviceContext()->PSSetShaderResources(0, 3, views);
	//D3D::Get()->GetDeviceContext()->PSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());
	//D3D::Get()->GetDeviceContext()->PSSetConstantBuffers(0, 1, cubeMappingConstantBuffer.GetAddressOf());

	//D3D::Get()->GetDeviceContext()->DrawIndexed(m_cubeMapping.CubeMesh->IndexCount, 0, 0);
}