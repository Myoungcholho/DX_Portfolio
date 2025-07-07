#include "Pch.h"
#include "TexturedGround.h"

void TextureGround::Initialize()
{
	transform = make_shared<Transform>();
	transform->SetPosition(Vector3(0.f, -20.f, 20.f));
	transform->SetRotation(Vector3(90.f, 0.f, 0.f));

	PBRMeshData ground = GeomtryGenerator::MakeSquareGrid(4096, 4096, 20.f, { 40.0f,40.0f });
	ground.albedoTextureFilename = "../../../_Textures/PBR/Bricks034_1K-PNG/""Bricks034_1K-PNG_Color.png";
	ground.normalTextureFilename = "../../../_Textures/PBR/Bricks034_1K-PNG/""Bricks034_1K-PNG_NormalDX.png";
	ground.heightTextureFilename = "../../../_Textures/PBR/Bricks034_1K-PNG/""Bricks034_1K-PNG_Displacement.png";
	ground.aoTextureFilename = "../../../_Textures/PBR/Bricks034_1K-PNG/""Bricks034_1K-PNG_AmbientOcclusion.png";

	// ConstantBuffer
	worldInvConstantBufferData.World = Matrix();
	worldInvConstantBufferData.InvTranspose = Matrix();

	D3D::Get()->CreateConstantBuffer(worldInvConstantBufferData, worldInvConstantBuffer);
	D3D::Get()->CreateConstantBuffer(heightMapConstantBufferData, heightMapConstantBuffer);
	D3D::Get()->CreateConstantBuffer(cameraConstantBufferData, cameraConstantBuffer);
	D3D::Get()->CreateConstantBuffer(materialConstantBufferData, materialConstantBuffer);
	D3D::Get()->CreateConstantBuffer(lightConstantBufferData, lightConstantBuffer);
	D3D::Get()->CreateConstantBuffer(renderOptionsConstantBufferData, renderOptionsConstantBuffer);

	// Vertex & Index Buffer
	D3D::Get()->CreateVertexBuffer(ground.vertices, vertexBuffer);
	D3D::Get()->CreateIndexBuffer(ground.indices, indexBuffer);
	m_vertexCount = UINT(ground.vertices.size());
	m_indexCount = UINT(ground.indices.size());

	// Texture Read
	if (ground.albedoTextureFilename.empty() == false)
	{
		D3D::Get()->CreateTexture(ground.albedoTextureFilename, albedoTexture, albedoTextureResourceView);
	}

	if (ground.normalTextureFilename.empty() == false)
	{
		D3D::Get()->CreateTexture(ground.normalTextureFilename, normalTexture, normalTextureResourceView);
	}

	if (ground.heightTextureFilename.empty() == false)
	{
		D3D::Get()->CreateTexture(ground.heightTextureFilename, heightTexture, heightTextureResourceView);
	}

	if (ground.aoTextureFilename.empty() == false)
	{
		D3D::Get()->CreateTexture(ground.aoTextureFilename, aoTexture, aoTextureResourceView);
	}

	// InputLayout
	vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	
	D3D::Get()->CreateVertexShaderAndInputLayout(L"BasicShader/BasicVertexShader.hlsl", basicInputElements,m_vertexShader, m_inputLayout);
	D3D::Get()->CreatePixelShader(L"BasicShader/BasicPixelShader.hlsl", m_pixelShader);
}

void TextureGround::Destroy()
{
}

void TextureGround::Tick()
{
	// world 계산
	worldInvConstantBufferData.World = transform->GetWorldMatrix().Transpose();
	worldInvConstantBufferData.InvTranspose = worldInvConstantBufferData.World;
	worldInvConstantBufferData.InvTranspose.Translation(Vector3(0.0f));
	worldInvConstantBufferData.InvTranspose = worldInvConstantBufferData.InvTranspose.Transpose().Invert();

	// eyeworld
	cameraConstantBufferData.eyeWorld = CContext::Get()->GetCamera()->GetPosition();

	// material
	// GUI로 제어

	// Light
	Vector3 curPos = transform->GetPosition();
	Engine::Get()->GetLightManager()->UpdateCBuffer(lightConstantBufferData, curPos, MAX_LIGHTS);

	D3D::Get()->UpdateBuffer(worldInvConstantBufferData, worldInvConstantBuffer);
	D3D::Get()->UpdateBuffer(heightMapConstantBufferData, heightMapConstantBuffer);
	D3D::Get()->UpdateBuffer(cameraConstantBufferData, cameraConstantBuffer);
	D3D::Get()->UpdateBuffer(materialConstantBufferData, materialConstantBuffer);
	D3D::Get()->UpdateBuffer(lightConstantBufferData, lightConstantBuffer);
	D3D::Get()->UpdateBuffer(renderOptionsConstantBufferData, renderOptionsConstantBuffer);
}

void TextureGround::UpdateGUI()
{
	ImGui::Begin("TextureGround");
	{
		// 텍스처 사용 유무
		bool temp = bool(renderOptionsConstantBufferData.useTexture);
		ImGui::Checkbox("Use Texture", &temp);
		renderOptionsConstantBufferData.useTexture = temp;

		temp = bool(renderOptionsConstantBufferData.useNormalMap);
		ImGui::Checkbox("Use Normal", &temp);
		renderOptionsConstantBufferData.useNormalMap = temp;

		temp = bool(renderOptionsConstantBufferData.useAOMap);
		ImGui::Checkbox("Use AO", &temp);
		renderOptionsConstantBufferData.useAOMap = temp;

		temp = bool(heightMapConstantBufferData.useHeightMap);
		ImGui::Checkbox("Use HeightMap", &temp);
		heightMapConstantBufferData.useHeightMap = temp;
		ImGui::SliderFloat("HeightScale", &heightMapConstantBufferData.heightScale, 0.0f, 1.0f);

		// Material
		ImGui::SliderFloat3("MaterialDiffuse", &materialConstantBufferData.material.diffuse.x, 0.0f, 1.0f);
		ImGui::SliderFloat3("MaterialSpecular", &materialConstantBufferData.material.specular.x, 0.0f, 1.0f);

		// transform 조절
		Vector3 pos = transform->GetPosition();
		if (ImGui::SliderFloat3("Position", &pos.x, -50.0f, 50.0f))
			transform->SetPosition(pos);

		Vector3 rot = transform->GetRotation();
		if (ImGui::SliderFloat3("Rotation", &rot.x, -180.0f, 180.0f))
			transform->SetRotation(rot);

		Vector3 scale = transform->GetScale();
		if (ImGui::SliderFloat3("Scale", &scale.x, 0.1f, 5.0f))
			transform->SetScale(scale);
	}
	ImGui::End();
}

void TextureGround::Render()
{
	ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();

	UINT stride = sizeof(FVertexPNTT);
	UINT offset = 0;

	// VS
	context->VSSetShader(m_vertexShader.Get(), 0, 0);
	context->VSSetShaderResources(0, 1, heightTextureResourceView.GetAddressOf());
	context->VSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());
	context->VSSetConstantBuffers(0, 1, worldInvConstantBuffer.GetAddressOf());
	context->VSSetConstantBuffers(2, 1, heightMapConstantBuffer.GetAddressOf());
	
	// PS
	context->PSSetShader(m_pixelShader.Get(), 0, 0);
	vector<ID3D11ShaderResourceView*> resViews =
	{
		Engine::Get()->GetTextureManager()->Get(L"SkyBox_Diffuse"),
		Engine::Get()->GetTextureManager()->Get(L"SkyBox_Specular"),
		albedoTextureResourceView.Get(),
		normalTextureResourceView.Get(),
		aoTextureResourceView.Get()
	};
	context->PSSetShaderResources(0, UINT(resViews.size()), resViews.data());

	context->PSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearWrapSampler());
	context->PSSetConstantBuffers(0, 1, cameraConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, materialConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(3, 1, lightConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(4, 1, renderOptionsConstantBuffer.GetAddressOf());

	// IA
	context->IASetInputLayout(m_inputLayout.Get());
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	context->DrawIndexed(m_indexCount, 0, 0);
}
