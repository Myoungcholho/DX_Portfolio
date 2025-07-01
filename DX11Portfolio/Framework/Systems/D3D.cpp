#include "Framework.h"
#include "D3D.h"

D3D* D3D::Instance = nullptr;
D3DDesc D3D::D3dDesc = D3DDesc();

void ReadImage(const std::string filename, std::vector<uint8_t>& image,
	int& width, int& height);

D3D* D3D::Get()
{
	assert(Instance != nullptr);
	return Instance;
}

void D3D::Create()
{
	assert(Instance == nullptr);
	Instance = new D3D();
}

void D3D::Destroy()
{
	if (Instance != nullptr)
	{
		delete Instance;
		Instance = nullptr;
	}
}

const D3DDesc& D3D::GetDesc()
{
	return D3dDesc;
}

void D3D::SetDesc(const D3DDesc& InDesc)
{
	D3dDesc = InDesc;
}

void D3D::CreateDevice()
{
	DXGI_MODE_DESC desc = {};
	desc.Width = (UINT)D3dDesc.Width;
	desc.Height = (UINT)D3dDesc.Height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.RefreshRate.Numerator = 0;
	desc.RefreshRate.Denominator = 1;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc = desc;
	swapChainDesc.BufferCount = 1;
	// 필터로도 사용할꺼라 추가
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	swapChainDesc.OutputWindow = D3dDesc.Handle;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// MSAA 설정 시 수정
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		SwapChain.GetAddressOf(),
		Device.GetAddressOf(),
		nullptr,
		DeviceContext.GetAddressOf()
	);
	assert(SUCCEEDED(hr) && "Device creation failed");
}

void D3D::CreateRTV()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	assert(SUCCEEDED(hr) && "Failed to get back buffer");

	// 렌더링 대상 뷰 생성
	hr = Device->CreateRenderTargetView(backBuffer.Get(), nullptr, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr) && "Failed to create RTV");

	// 렌더링 끝난 이미지를 가져다 후처리할 SRV 생성
	hr = Device->CreateShaderResourceView(backBuffer.Get(), nullptr, ShaderResourceView.GetAddressOf());
	assert(SUCCEEDED(hr) && "Failed to create SRV");
}

void D3D::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;
	desc.DepthClipEnable = true;			// MinDepth ~ MaxDepth 밖이라면 그리지 않겠다!

	HRESULT hr = Device->CreateRasterizerState(&desc, RasterizerState.GetAddressOf());
	assert(SUCCEEDED(hr));

	DeviceContext->RSSetState(RasterizerState.Get());
}

void D3D::CreateDSV()
{
	DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = (UINT)D3dDesc.Width;
	texDesc.Height = (UINT)D3dDesc.Height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	// MSAA 설정 시 변경
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// GPU의 메모리 할당이기 때문에, Device인 인터페이스를 통해 메모리를 생성
	if (Device->CreateTexture2D(&texDesc, nullptr, DSV_Texture.GetAddressOf()))
		return;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	if (Device->CreateDepthStencilView(DSV_Texture.Get(), &dsvDesc, DepthStencilView.GetAddressOf()))
		return;
}

void D3D::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	dsDesc.StencilEnable = false; // true면 Stencil 연산도 정의 필요

	HRESULT hr = Device->CreateDepthStencilState(&dsDesc, DepthStencilState.GetAddressOf());
	assert(SUCCEEDED(hr));

	DeviceContext->OMSetDepthStencilState(DepthStencilState.Get(), 0);
}

//void D3D::CreateNoDepthStencilState()
//{
//	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
//	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
//	dsDesc.DepthEnable = true;
//	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
//
//	dsDesc.StencilEnable = false; // true면 Stencil 연산도 정의 필요
//
//	HRESULT hr = Device->CreateDepthStencilState(&dsDesc, DepthStencilState.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	DeviceContext->OMSetDepthStencilState(DepthStencilState.Get(), 0);
//}

void D3D::CreateIndexBuffer(const std::vector<uint16_t>& indices, ComPtr<ID3D11Buffer>& m_indexBuffer)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
	bufferDesc.ByteWidth = UINT(sizeof(uint16_t) * indices.size());
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
	bufferDesc.StructureByteStride = sizeof(uint16_t);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = indices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	Device->CreateBuffer(&bufferDesc, &indexBufferData, m_indexBuffer.GetAddressOf());
}

void D3D::CreateIndexBuffer(const std::vector<uint32_t>& indices, ComPtr<ID3D11Buffer>& m_indexBuffer)
{
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = indices.data();

	Device->CreateBuffer(&desc, &data, m_indexBuffer.GetAddressOf());
}

void D3D::SetRenderTarget()
{
	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
}

void D3D::SetDepthStencilState()
{
	DeviceContext->OMSetDepthStencilState(DepthStencilState.Get(), 0);
}

ComPtr<ID3D11RenderTargetView> D3D::GetMainRenderTargetView()
{
	return RenderTargetView;
}

ComPtr<ID3D11ShaderResourceView> D3D::GetMainShaderResourceView()
{
	return ShaderResourceView;
}

void D3D::ClearRenderTargetView(Color InColor)
{
	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), InColor);
}

void D3D::ClearDepthStencilView()
{
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void D3D::Present()
{
	SwapChain->Present(1, 0);
}

void D3D::ResizeScreen(float InWidth, float InHeight)
{
	if (InWidth < 1 || InHeight < 1)
		return;

	D3dDesc.Width = InWidth;
	D3dDesc.Height = InHeight;

	RenderTargetView.Reset();
	DSV_Texture.Reset();
	DepthStencilView.Reset();

	SwapChain->ResizeBuffers(0, (UINT)InWidth, (UINT)InHeight, DXGI_FORMAT_UNKNOWN, 0);

	CreateRTV();
	CreateDSV();
	
	if (OnReSizeDelegate.IsBound())
		OnReSizeDelegate.Broadcast();
}

ID3D11Texture2D* D3D::GetRTVTexture()
{
	ComPtr<ID3D11Texture2D> texture;
	RenderTargetView->GetResource(reinterpret_cast<ID3D11Resource**>(texture.GetAddressOf()));
	return texture.Detach();
}

void CheckResult(HRESULT hr, ID3DBlob* errorBlob) 
{
	if (FAILED(hr)) 
	{
		if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) 
		{
			cout << "File not found." << endl;
		}
		if (errorBlob) 
		{
			cout << "Shader compile error\n" << (char*)errorBlob->GetBufferPointer() << endl;
		}
	}
}

void D3D::CreateVertexShaderAndInputLayout(const wstring& filename, const vector<D3D11_INPUT_ELEMENT_DESC>& inputElements, ComPtr<ID3D11VertexShader>& vertexShader, ComPtr<ID3D11InputLayout>& inputLayout)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	const wstring fileName = L"../Framework/Shaders/" + filename;
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	HRESULT hr =
		D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());

	Device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
		&vertexShader);

	Device->CreateInputLayout(inputElements.data(), UINT(inputElements.size()),
		shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
		&inputLayout);
}

void D3D::CreateHullShader(const wstring& filename, ComPtr<ID3D11HullShader>& hullShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	const wstring fileName = L"../Framework/Shaders/" + filename;
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	HRESULT hr = 
		D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "hs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);
	CheckResult(hr, errorBlob.Get());

	Device->CreateHullShader(shaderBlob->GetBufferPointer(),shaderBlob->GetBufferSize(), NULL, &hullShader);
}

void D3D::CreateDomainShader(const wstring& filename, ComPtr<ID3D11DomainShader>& domainShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	const wstring fileName = L"../Framework/Shaders/" + filename;
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	HRESULT hr = 
		D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main","ds_5_0", compileFlags, 0, &shaderBlob, &errorBlob);
	CheckResult(hr, errorBlob.Get());

	Device->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &domainShader);
}

void D3D::CreatePixelShader(const wstring& filename, ComPtr<ID3D11PixelShader>& pixelShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	const wstring fileName = L"../Framework/Shaders/" + filename;
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	HRESULT hr =
		D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());

	Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
		&pixelShader);
}

void D3D::CreateGeometryShader(const wstring& filename, ComPtr<ID3D11GeometryShader>& geometeyShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	const wstring fileName = L"../Framework/Shaders/" + filename;
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	HRESULT hr =
		D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());
	
	Device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
		&geometeyShader);
}

void D3D::CreateTextureArray(vector<string> filenames, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& textureResourceView)
{
	int width = 0, height = 0;
	vector<uint8_t> imageArray;
	for (const auto& f : filenames)
	{
		std::vector<uint8_t> image;

		ReadImage(f, image, width, height);

		imageArray.insert(imageArray.begin(), image.begin(), image.end());
	}

	// Create texture.
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = UINT(width);
	txtDesc.Height = UINT(height);
	txtDesc.MipLevels = 1;
	txtDesc.ArraySize = UINT(filenames.size());
	txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.SampleDesc.Quality = 0;
	txtDesc.Usage = D3D11_USAGE_IMMUTABLE;
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// SUBRESOURCE_DATA의 배열
	std::vector<D3D11_SUBRESOURCE_DATA> initData(filenames.size());
	size_t offset = 0;
	for (auto& i : initData) {
		i.pSysMem = imageArray.data() + offset;
		i.SysMemPitch = txtDesc.Width * sizeof(uint8_t) * 4;
		i.SysMemSlicePitch = txtDesc.Width * txtDesc.Height * sizeof(uint8_t) * 4;
		offset += i.SysMemSlicePitch;
	}

	Device->CreateTexture2D(&txtDesc, initData.data(), texture.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = txtDesc.Format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.MostDetailedMip = 0;
	desc.Texture2DArray.MipLevels = txtDesc.MipLevels;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.ArraySize = txtDesc.ArraySize;

	Device->CreateShaderResourceView(texture.Get(), &desc,
		textureResourceView.GetAddressOf());
}


D3D::D3D()
{
	CreateDevice();
	CreateRTV();
	CreateDSV();
	
	CreateRasterizerState();
	CreateDepthStencilState();
}

D3D::~D3D()
{
	DepthStencilView.Reset();
	DSV_Texture.Reset();
	RenderTargetView.Reset();
	DeviceContext.Reset();
	Device.Reset();
	SwapChain.Reset();
	RasterizerState.Reset();
	DepthStencilState.Reset();
}

///////////////////////////////////////////////////////////////////////////////
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

void ReadImage(const std::string filename, std::vector<uint8_t>& image,
	int& width, int& height) {

	int channels;

	unsigned char* img =
		stbi_load(filename.c_str(), &width, &height, &channels, 0);

	// assert(channels == 4);

	// 4채널로 만들어서 복사
	image.resize(width * height * 4);

	if (channels == 3) {
		for (size_t i = 0; i < width * height; i++) {
			for (size_t c = 0; c < 3; c++) {
				image[4 * i + c] = img[i * channels + c];
			}
			image[4 * i + 3] = 255;
		}
	}
	else if (channels == 4) {
		for (size_t i = 0; i < width * height; i++) {
			for (size_t c = 0; c < 4; c++) {
				image[4 * i + c] = img[i * channels + c];
			}
		}
	}
	else {
		std::cout << "Read 3 or 4 channels images only. " << channels
			<< " channels" << endl;
	}
}