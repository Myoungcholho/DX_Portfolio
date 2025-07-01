#include "Pch.h"
#include "ImageFilter.h"

ImageFilter::ImageFilter(const wstring vertexPrefix, const wstring pixelPrefix)
{
	m_vertexPrefix = vertexPrefix;
	m_pixelPrefix = pixelPrefix;

	m_width = D3D::Get()->GetDesc().Width;
	m_height = D3D::Get()->GetDesc().Height;
	Initialize();
}

ImageFilter::ImageFilter(const wstring vertexPrefix, const wstring pixelPrefix, int width, int height)
{
	m_vertexPrefix = vertexPrefix;
	m_pixelPrefix = pixelPrefix;

	m_width = width;
	m_height = height;

	Initialize();
}

void ImageFilter::Initialize()
{
	MeshData meshData = GeomtryGenerator::MakeSquare();
	m_mesh = make_shared<FMesh>();

	D3D::Get()->CreateVertexBuffer(meshData.vertices, m_mesh->VertexBuffer);
	m_mesh->IndexCount = UINT(meshData.indices.size());
	D3D::Get()->CreateIndexBuffer(meshData.indices, m_mesh->IndexBuffer);

	vector<D3D11_INPUT_ELEMENT_DESC> basicInputElement =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3D::Get()->CreateVertexShaderAndInputLayout
	(
		m_vertexPrefix + L"VertexShader.hlsl",
		basicInputElement,
		m_vertexShader,
		m_inputLayout
	);

	D3D::Get()->CreatePixelShader(m_pixelPrefix + L"PixelShader.hlsl", m_pixelShader);

	// Sampler는 Graphics에서 정의해서 PASS
	// RSState 도 PASS

	ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = float(m_width);
	m_viewport.Height = float(m_height);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	D3D::Get()->OnReSizeDelegate.AddDynamic(this, &ImageFilter::ReSizeScreen, "ReSizeScreen");

	// 필터 RTV 정의
	ComPtr<ID3D11Texture2D> texture;

	// 텍스쳐 정보 정의
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = m_width;
	txtDesc.Height = m_height;
	txtDesc.MipLevels = txtDesc.ArraySize = 1;
	txtDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_DEFAULT;
	// 셰이더에서 읽기 가능 | 이 텍스처에 렌더링 가능 | UAV사용 가능 ComputeShader 사용 가능 |
	txtDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	txtDesc.MiscFlags = 0;
	txtDesc.CPUAccessFlags = 0;

	//렌더타겟뷰 정보 정의
	D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
	viewDesc.Format = txtDesc.Format;
	viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipSlice = 0;

	// 텍스처 생성 & RTV & SRV 생성
	D3D::Get()->GetDevice()->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());
	D3D::Get()->GetDevice()->CreateRenderTargetView(texture.Get(), &viewDesc, m_renderTargetView.GetAddressOf());
	D3D::Get()->GetDevice()->CreateShaderResourceView(texture.Get(), nullptr, m_shaderResourceView.GetAddressOf());

	// PS에서 이웃 픽셀로 접근하기 위한 값 전달
	m_pixelConstData.dx = 1.0f / m_width;
	m_pixelConstData.dy = 1.0f / m_height;

	D3D::Get()->CreateConstantBuffer(m_pixelConstData, m_mesh->PSCBuffer);

	// 새로 만든 텍스처를 RTV로 쓰겠다.
	SetRenderTargets({ m_renderTargetView });
}

void ImageFilter::Destroy()
{
	D3D::Get()->OnReSizeDelegate.RemoveDynamic(this, "ReSizeScreen");
}

void ImageFilter::UpdateGUI()
{

}

void ImageFilter::Tick()
{
	// 화면이 바뀔수도 있어서 추가
	m_pixelConstData.dx = 1.0f / m_width;
	m_pixelConstData.dy = 1.0f / m_height;

	D3D::Get()->UpdateBuffer(m_pixelConstData, m_mesh->PSCBuffer);
}

void ImageFilter::Render()
{
	assert(m_shaderResources.size() > 0);
	assert(m_renderTargets.size() > 0);

	ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();

	context->OMSetRenderTargets(UINT(m_renderTargets.size()), m_renderTargets.data(), nullptr);
	// 뎊스 설정

	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
	context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);

	GraphicsDevice::Get()->ApplyNoCullNoClipRasterizer();
	context->RSSetViewports(1, &m_viewport);

	UINT stride = sizeof(FVertex);
	UINT offset = 0;

	// IA Setting
	context->IASetInputLayout(m_inputLayout.Get());
	context->IASetVertexBuffers(0, 1, m_mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Shader Setting
	context->VSSetShader(m_vertexShader.Get(), 0, 0);
	context->PSSetShader(m_pixelShader.Get(), 0, 0);

	// Shader Data
	context->PSSetShaderResources(0, UINT(m_shaderResources.size()), m_shaderResources.data());
	context->PSSetConstantBuffers(0, 1, m_mesh->PSCBuffer.GetAddressOf());
	context->PSSetSamplers(0, 1, GraphicsDevice::Get()->GetLinearClampSampler());

	// Draw
	context->DrawIndexed(m_mesh->IndexCount, 0, 0);
}

void ImageFilter::SetShaderResources(const vector<ComPtr<ID3D11ShaderResourceView>>& resources)
{
	m_shaderResources.clear();
	for (const auto& res : resources)
	{
		m_shaderResources.push_back(res.Get());
	}
}

void ImageFilter::SetRenderTargets(const vector<ComPtr<ID3D11RenderTargetView>>& targets)
{
	m_renderTargets.clear();
	for (const auto& tar : targets)
	{
		m_renderTargets.push_back(tar.Get());
	}
}

void ImageFilter::ReSizeScreen()
{
	cout << "RESIZE CALL!" << "\n";

	//m_width = D3D::Get()->GetDesc().Width;
	//m_height = D3D::Get()->GetDesc().Height;
}