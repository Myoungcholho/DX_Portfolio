#include "Framework.h"
#include "D3D.h"
#include "DirectXTK/DDSTextureLoader.h"

D3D* D3D::Instance = nullptr;
D3DDesc D3D::D3dDesc = D3DDesc();

void ReadImage(const std::string filename, std::vector<uint8_t>& image,int& width, int& height);
void ReadEXRImage(const std::string filename, std::vector<uint8_t>& image, int& width, int& height, DXGI_FORMAT& pixelFormat);
void CreateTextureHelper
(
	ComPtr<ID3D11Device>& device,
	ComPtr<ID3D11DeviceContext>& context,
	const int width, const int height,
	const vector<uint8_t>& image,
	const DXGI_FORMAT pixelFormat,
	ComPtr<ID3D11Texture2D>& texture,
	ComPtr<ID3D11ShaderResourceView>& srv
);

ComPtr<ID3D11Texture2D> CreateStagingTexture(const int width, const int height, const std::vector<uint8_t>& image, 
	const DXGI_FORMAT pixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
	const int mipLevels = 1, const int arraySize = 1);

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

void D3D::SaveAndBindTempDepthStencil()
{
	// 1. 가지고 와서 temp에 DSV를 복사한다
	DeviceContext->CopyResource(Temp_DSV_Texture.Get(), DSV_Texture.Get());

	// 2. 원본 DSV정보는 유지할수있도록하기 위해 복사한 DSV를 사용한다
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	vector<ID3D11RenderTargetView*> renderTargetViews = { m_floatRTV.Get() };
	DeviceContext->OMSetRenderTargets(UINT(renderTargetViews.size()), renderTargetViews.data(), Temp_DepthStencilView.Get());
}

void D3D::RestoreOriginalDepthStencil()
{
	// 1. 원래 쓰던 DSV를 OM에 바인딩
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	vector<ID3D11RenderTargetView*> renderTargetViews = { m_floatRTV.Get() };
	DeviceContext->OMSetRenderTargets(UINT(renderTargetViews.size()), renderTargetViews.data(), DepthStencilView.Get());
}

void D3D::CreateDevice()
{
	DXGI_MODE_DESC desc = {};
	desc.Width = (UINT)D3dDesc.Width;
	desc.Height = (UINT)D3dDesc.Height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.RefreshRate.Numerator = 0;			// 0은 자동으로 현재 모니터 주사율 사용
	desc.RefreshRate.Denominator = 1;		// 주사율 분모 현재 0(자동->75)/1 = 75hz

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc = desc;
	swapChainDesc.BufferCount = 2;			// 백버퍼 사용 개수
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = D3dDesc.Handle;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;


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

void D3D::CreateBuffers()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	// 1. 스왑체인으로부터 백버퍼를 얻고 텍스처에 저장
	ThrowIfFailed(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	// 2. 백버퍼로 RTV를 만듬
	ThrowIfFailed(Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_backBufferRTV.GetAddressOf()));
	// 3. 내 GPU 드라이버가 제공하는 품질 수준 가능 범위 반환 및 float텍스처 생성
	ThrowIfFailed(Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &m_numQualityLevels));
	D3D11_TEXTURE2D_DESC desc;
	backBuffer->GetDesc(&desc);
	desc.MipLevels = desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;
	if (m_useMSAA && m_numQualityLevels)
	{
		desc.SampleDesc.Count = 4;
		desc.SampleDesc.Quality = m_numQualityLevels - 1;
	}
	else
	{
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
	}

	ThrowIfFailed(Device->CreateTexture2D(&desc, NULL, m_floatBuffer.GetAddressOf()));
	// 4. float Texture로 SRV 생성
	Device->CreateShaderResourceView(m_floatBuffer.Get(), NULL, m_floatSRV.GetAddressOf());
	// 5. float Texture로 RTV 생성
	Device->CreateRenderTargetView(m_floatBuffer.Get(), NULL, m_floatRTV.GetAddressOf());
	// 6. DepthBuffer 생성
	CreateDepthBuffer();
	
	// 7. PostProcess를 위한 MSAA 제거 SRV/RTV
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	ThrowIfFailed(Device->CreateTexture2D(&desc, NULL, m_resolvedBuffer.GetAddressOf()));
	ThrowIfFailed(Device->CreateShaderResourceView(m_resolvedBuffer.Get(), NULL, m_resolvedSRV.GetAddressOf()));
	ThrowIfFailed(Device->CreateRenderTargetView(m_resolvedBuffer.Get(), NULL, m_resolvedRTV.GetAddressOf()));
}

void D3D::CreateDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	depthStencilBufferDesc.Width = (UINT)D3dDesc.Width;
	depthStencilBufferDesc.Height = (UINT)D3dDesc.Height;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (m_numQualityLevels > 0)
	{
		depthStencilBufferDesc.SampleDesc.Count = 4;
		depthStencilBufferDesc.SampleDesc.Quality = m_numQualityLevels - 1;
	}
	else
	{
		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;
	}
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	Device->CreateTexture2D(&depthStencilBufferDesc, 0, DSV_Texture.GetAddressOf());
	Device->CreateDepthStencilView(DSV_Texture.Get(), 0, DepthStencilView.GetAddressOf());

	Device->CreateTexture2D(&depthStencilBufferDesc, 0, Temp_DSV_Texture.GetAddressOf());
	Device->CreateDepthStencilView(Temp_DSV_Texture.Get(), 0, Temp_DepthStencilView.GetAddressOf());
}

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

ComPtr<ID3D11RenderTargetView> D3D::GetMainRenderTargetView()
{
	return m_backBufferRTV;
}

ComPtr<ID3D11ShaderResourceView> D3D::GetMainShaderResourceView()
{
	return ShaderResourceView;
}

void D3D::StartRenderPass()
{
	// RTV
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	vector<ID3D11RenderTargetView*> renderTargetViews = { m_floatRTV.Get() };
	for (size_t i = 0; i < renderTargetViews.size(); ++i)
	{
		DeviceContext->ClearRenderTargetView(renderTargetViews[i], clearColor);
	}
	DeviceContext->OMSetRenderTargets(UINT(renderTargetViews.size()), renderTargetViews.data(), DepthStencilView.Get());

	// DSV
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f, 0);
}

void D3D::ClearMainDepth(D3D11_CLEAR_FLAG ClearFlags,FLOAT Depth,UINT8 Stencil)
{
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), ClearFlags, Depth, Stencil);
}

void D3D::ClearTempDepth(D3D11_CLEAR_FLAG ClearFlags, FLOAT Depth, UINT8 Stencil)
{
	DeviceContext->ClearDepthStencilView(Temp_DepthStencilView.Get(), ClearFlags, Depth, Stencil);
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

	m_backBufferRTV.Reset();
	DSV_Texture.Reset();
	DepthStencilView.Reset();

	SwapChain->ResizeBuffers(0, (UINT)InWidth, (UINT)InHeight, DXGI_FORMAT_UNKNOWN, 0);

	CreateBuffers();
	m_postProcess.Initialize({ m_resolvedSRV }, { m_backBufferRTV }, D3dDesc.Width, D3dDesc.Height, 4);

	if (OnReSizeDelegate.IsBound())
		OnReSizeDelegate.Broadcast();
}

ID3D11Texture2D* D3D::GetRTVTexture()
{
	ComPtr<ID3D11Texture2D> texture;
	m_backBufferRTV->GetResource(reinterpret_cast<ID3D11Resource**>(texture.GetAddressOf()));
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

	//const wstring fileName = L"../Framework/Shaders/" + filename;
	const wstring fileName = L"C:/DirectX/Portfolio/DX11Portfolio/Framework/Shaders/" + filename;
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS;

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

	//const wstring fileName = L"../Framework/Shaders/" + filename;
	const wstring fileName = L"C:/DirectX/Portfolio/DX11Portfolio/Framework/Shaders/" + filename;

	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS;

	HRESULT hr = 
		D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "hs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);
	CheckResult(hr, errorBlob.Get());

	Device->CreateHullShader(shaderBlob->GetBufferPointer(),shaderBlob->GetBufferSize(), NULL, &hullShader);
}

void D3D::CreateDomainShader(const wstring& filename, ComPtr<ID3D11DomainShader>& domainShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	//const wstring fileName = L"../Framework/Shaders/" + filename;
	const wstring fileName = L"C:/DirectX/Portfolio/DX11Portfolio/Framework/Shaders/" + filename;
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS;

	HRESULT hr = 
		D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main","ds_5_0", compileFlags, 0, &shaderBlob, &errorBlob);
	CheckResult(hr, errorBlob.Get());

	Device->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &domainShader);
}

void D3D::CreatePixelShader(const wstring& filename, ComPtr<ID3D11PixelShader>& pixelShader)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;

	//const wstring fileName = L"../Framework/Shaders/" + filename;
	const wstring fileName = L"C:/DirectX/Portfolio/DX11Portfolio/Framework/Shaders/" + filename;
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS;

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

	//const wstring fileName = L"../Framework/Shaders/" + filename;
	const wstring fileName = L"C:/DirectX/Portfolio/DX11Portfolio/Framework/Shaders/" + filename;
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS;

	HRESULT hr =
		D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

	CheckResult(hr, errorBlob.Get());
	
	Device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
		&geometeyShader);
}

void D3D::CreateTexture(const string filename, const bool useSRGB,
	ComPtr<ID3D11Texture2D>& texture, 
	ComPtr<ID3D11ShaderResourceView>& textureResourceView)
{
	int width, height;
	std::vector<uint8_t> image;
	DXGI_FORMAT pixelFormat = useSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;

	string ext(filename.end() - 3, filename.end());
	std::transform(ext.begin(), ext.end(), ext.begin(),[](unsigned char c) { return std::tolower(c); });

	if (ext == "exr")
	{
		ReadEXRImage(filename, image, width, height, pixelFormat);
	}
	else
	{
		ReadImage(filename, image, width, height);
	}

	CreateTextureHelper(Device, DeviceContext, width, height, image, pixelFormat, texture, textureResourceView);
}

void D3D::CreateTextureArray(vector<string> filenames, const bool useSRGB, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& textureResourceView)
{
	if (filenames.empty())
		return;

	int width = 0, height = 0;
	vector<vector<uint8_t>> imageArray;
	for (const auto& f : filenames)
	{
		std::vector<uint8_t> image;

		ReadImage(f, image, width, height);

		imageArray.push_back(image);
	}

	// Create texture.
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = UINT(width);
	txtDesc.Height = UINT(height);
	txtDesc.MipLevels = 0;			// 밉맵 최대(수정)
	txtDesc.ArraySize = UINT(filenames.size());

	if(useSRGB == true)
		txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	else
		txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	txtDesc.SampleDesc.Count = 1;
	txtDesc.SampleDesc.Quality = 0;
	txtDesc.Usage = D3D11_USAGE_DEFAULT; // stating texutre로부터 복사 (수정)
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // 렌더타겟 추가 (수정)
	txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 밉맵 사용(수정)

	// 초기 데이터 없이 텍스춰를 만듭니다.
	D3D::Get()->GetDevice()->CreateTexture2D(&txtDesc, nullptr, texture.GetAddressOf());

	// 실제로 만들어진 MipLevels를 확인
	texture->GetDesc(&txtDesc);
	// cout << txtDesc.MipLevels << endl;

	// StagingTexture를 만들어서 하나씩 복사합니다.
	for (size_t i = 0; i < imageArray.size(); i++) 
	{

		auto& image = imageArray[i];

		// StagingTexture는 Texture2DArray가 아니라 Texture2D
		ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(width, height, image, txtDesc.Format, 1, 1);

		// 스테이징 텍스춰를 텍스춰 배열의 해당 위치에 복사합니다.
		UINT subresourceIndex = D3D11CalcSubresource(0, UINT(i), txtDesc.MipLevels);

		D3D::Get()->GetDeviceContext()->CopySubresourceRegion(texture.Get(), subresourceIndex, 0, 0, 0, stagingTexture.Get(), 0, nullptr);
	}

	D3D::Get()->GetDevice()->CreateShaderResourceView(texture.Get(), nullptr,textureResourceView.GetAddressOf());

	D3D::Get()->GetDeviceContext()->GenerateMips(textureResourceView.Get());
}

void D3D::CreateDDSTexture(const wchar_t* filename, bool isCubeMap, ComPtr<ID3D11ShaderResourceView>& textureResourceView)
{
	ComPtr<ID3D11Texture2D> texture;

	UINT miscFlags = 0;
	if (isCubeMap)
		miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	
	ThrowIfFailed(CreateDDSTextureFromFileEx
	(
		Device.Get(), 
		filename, 
		0, 
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE, 
		0, 
		miscFlags, 
		DDS_LOADER_FLAGS(false),
		(ID3D11Resource**)texture.GetAddressOf(),
		textureResourceView.GetAddressOf(), NULL)
	);
}

void D3D::CreateMetallicRoughnessTexture(const string metallicFilename, const string roughnessFilename, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& srv)
{
	// 메탈릭파일이 있는데 그게 메탈릭과 러프니스가 이름이 같다면 합쳐져 있는 것
	if (!metallicFilename.empty() && (metallicFilename == roughnessFilename)) 
	{
		CreateTexture(metallicFilename, false, texture, srv);
	}
	else // 아니라면
	{
		
		int mWidth = 0, mHeight = 0;
		int rWidth = 0, rHeight = 0;
		std::vector<uint8_t> mImage;	
		std::vector<uint8_t> rImage;

		// 메탈릭 이미지 읽음
		if (!metallicFilename.empty()) 
		{
			ReadImage(metallicFilename, mImage, mWidth, mHeight);
		}

		// 러프니스 이미지 읽음
		if (!roughnessFilename.empty()) 
		{
			ReadImage(roughnessFilename, rImage, rWidth, rHeight);
		}

		// 만약 둘다 읽었다면 크기가 같은지 확인
		if (!metallicFilename.empty() && !roughnessFilename.empty()) 
		{
			assert(mWidth == rWidth);
			assert(mHeight == rHeight);
		}

		// RGBA사용하는 이미지 데이터 만듬
		// [0] : R , [1] : G, [2] : B, [3] : A
		// 따라서 [1]과 [2]만 사용함 그렇게 만든 데이터(이미지)로 텍스처를 생성
		vector<uint8_t> combinedImage(size_t(mWidth * mHeight) * 4);
		fill(combinedImage.begin(), combinedImage.end(), 0);

		for (size_t i = 0; i < size_t(mWidth * mHeight); i++) 
		{
			if (rImage.size())
				combinedImage[4 * i + 1] = rImage[4 * i];
			if (mImage.size())
				combinedImage[4 * i + 2] = mImage[4 * i];
		}

		// 합쳐진 이미지 데이터로 2D 텍스처 및 SRV 생성
		CreateTextureHelper(Device, DeviceContext, mWidth, mHeight, combinedImage, DXGI_FORMAT_R8G8B8A8_UNORM, texture, srv);
	}
}

D3D::D3D()
{
	CreateDevice();				// SwapChain, Device, Context 생성
	CreateBuffers();			// RTV들 생성
	
	//CreateRasterizerState();
	//CreateDepthStencilState();
}

D3D::~D3D()
{
	DepthStencilView.Reset();
	DSV_Texture.Reset();
	m_backBufferRTV.Reset();
	DeviceContext.Reset();
	Device.Reset();
	SwapChain.Reset();
	//RasterizerState.Reset();
}

void D3D::Init()
{
	m_postProcess.Initialize({ m_resolvedSRV }, { m_backBufferRTV }, D3dDesc.Width, D3dDesc.Height, 4);
}

void D3D::UpdateGUI()
{
	m_postProcess.UpdateGUI();
}

void D3D::Render()
{
	DeviceContext->ResolveSubresource(m_resolvedBuffer.Get(), 0, m_floatBuffer.Get(), 0, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_postProcess.Render();
}

///////////////////////////////////////////////////////////////////////////////
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <DirectXTexEXR.h>
#include <fp16.h>

void ReadImage(const std::string filename, std::vector<uint8_t>& image,
	int& width, int& height) {

	int channels;

	unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, 0);

	/*cout << filename << " " << width << " " << height << " " << channels
		<< endl;*/

	// 4채널로 만들어서 복사
	image.resize(width * height * 4);

	if (channels == 1) // [흑백 유무]
	{
		for (size_t i = 0; i < width * height; i++) 
		{
			uint8_t g = img[i * channels + 0];
			for (size_t c = 0; c < 4; c++) 
			{
				image[4 * i + c] = g;
			}
		}
	}
	else if (channels == 2) // 2채널은 RGB 값/A 값 으로 2개로 나눈것 [밝기/알파]
	{
		for (size_t i = 0; i < width * height; ++i)
		{
			uint8_t gray = img[i * channels];
			uint8_t alpha = img[i * channels + 1];

			image[4 * i + 0] = gray;   // R
			image[4 * i + 1] = gray;   // G
			image[4 * i + 2] = gray;   // B
			image[4 * i + 3] = alpha;  // A
		}
	}
	else if (channels == 3)  // [RGB]
	{
		for (size_t i = 0; i < width * height; i++) 
		{
			for (size_t c = 0; c < 3; c++) 
			{
				image[4 * i + c] = img[i * channels + c];
			}
			image[4 * i + 3] = 255;
		}
	}
	else if (channels == 4) // [RGBA]
	{
		for (size_t i = 0; i < width * height; i++) 
		{
			for (size_t c = 0; c < 4; c++) {
				image[4 * i + c] = img[i * channels + c];
			}
		}
	}
	else 
	{
		std::cout << "Cannot read " << channels << " channels" << endl;
	}
}

void ReadEXRImage(const std::string filename, std::vector<uint8_t>& image,
	int& width, int& height, DXGI_FORMAT& pixelFormat)
{
	const std::wstring wFilename(filename.begin(), filename.end());

	TexMetadata metadata;
	ThrowIfFailed(GetMetadataFromEXRFile(wFilename.c_str(), metadata));

	ScratchImage scratchImage;
	ThrowIfFailed(LoadFromEXRFile(wFilename.c_str(), NULL, scratchImage));

	width = static_cast<int>(metadata.width);
	height = static_cast<int>(metadata.height);
	pixelFormat = metadata.format;

	//cout << filename << " " << metadata.width << " " << metadata.height		<< metadata.format << endl;

	image.resize(scratchImage.GetPixelsSize());
	memcpy(image.data(), scratchImage.GetPixels(), image.size());

	// 데이터 범위 확인해보기
	vector<float> f32(image.size() / 2);
	uint16_t* f16 = (uint16_t*)image.data();
	for (int i = 0; i < image.size() / 2; i++) 
	{
		f32[i] = fp16_ieee_to_fp32_value(f16[i]);
	}

	const float minValue = *std::min_element(f32.begin(), f32.end());
	const float maxValue = *std::max_element(f32.begin(), f32.end());

	// cout << minValue << " " << maxValue << endl;
}

ComPtr<ID3D11Texture2D> CreateStagingTexture(const int width,const int height, const std::vector<uint8_t>& image,
	const DXGI_FORMAT pixelFormat,
	const int mipLevels, const int arraySize)
{
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = mipLevels;
	txtDesc.ArraySize = arraySize;
	txtDesc.Format = pixelFormat;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_STAGING;
	txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

	ComPtr<ID3D11Texture2D> stagingTexture;
	if (FAILED(D3D::Get()->GetDevice()->CreateTexture2D(&txtDesc, nullptr,stagingTexture.GetAddressOf()))) 
	{
		cout << "Failed()" << endl;
	}

	// CPU에서 이미지 데이터 복사
	D3D11_MAPPED_SUBRESOURCE ms;
	//(① 리소스(텍스처 등), ② 어떤 서브리소스 ? (보통 0), ③ 읽기 / 쓰기 모드, ④ 특별한 플래그(보통 0), ⑤ CPU가 접근할 수 있는 포인터 정보)
	D3D::Get()->GetDeviceContext()->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
	uint8_t* pData = (uint8_t*)ms.pData;
	for (UINT h = 0; h < UINT(height); h++) 
	{
		memcpy(&pData[h * ms.RowPitch], &image[h * width * 4],width * sizeof(uint8_t) * 4);
	}
	D3D::Get()->GetDeviceContext()->Unmap(stagingTexture.Get(), NULL);

	return stagingTexture;
}

void CreateTextureHelper
(
	ComPtr<ID3D11Device>& device,
	ComPtr<ID3D11DeviceContext>& context, 
	const int width, const int height,
	const vector<uint8_t>& image,
	const DXGI_FORMAT pixelFormat,
	ComPtr<ID3D11Texture2D>& texture,
	ComPtr<ID3D11ShaderResourceView>& srv
) 
{

	// CPU 접근용으로 이미지 복사
	ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(width, height, image, pixelFormat);

	// 실제로 사용할 텍스춰 설정
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = 0; // 밉맵 레벨 최대
	txtDesc.ArraySize = 1;
	txtDesc.Format = pixelFormat;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_DEFAULT; // 스테이징 텍스춰로부터 복사 가능
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 밉맵 사용
	txtDesc.CPUAccessFlags = 0;

	// 초기 데이터 없이 텍스춰 생성 (전부 검은색)
	device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());

	// 실제로 생성된 MipLevels를 확인해보고 싶을 경우
	// texture->GetDesc(&txtDesc);
	// cout << txtDesc.MipLevels << endl;

	// 스테이징 텍스춰로부터 가장 해상도가 높은 이미지 복사
	context->CopySubresourceRegion(texture.Get(), 0, 0, 0, 0,stagingTexture.Get(), 0, NULL);

	// 쉐이더에서 사용할 SRV 생성
	device->CreateShaderResourceView(texture.Get(), 0, srv.GetAddressOf());

	// 텍스처에 밉맵 레벨을 생성
	context->GenerateMips(srv.Get());
}