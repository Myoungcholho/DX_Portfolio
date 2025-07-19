#pragma once

#include <iostream>
#include <iomanip>

struct D3DDesc 
{
	HWND Handle; // SwapChain, Resize, Present에서 윈도우 핸들 사용하기 때문

	float Width;
	float Height;
	bool VSync;
};

inline void ThrowIfFailed(HRESULT hr) 
{
	if (FAILED(hr)) 
	{
		throw std::exception();
	}
}

class D3D
{
public:
	static D3D* Get();

public:
	static void Create();
	static void Destroy();

public:
	static const D3DDesc& GetDesc();
	static void SetDesc(const D3DDesc& InDesc);

private:
	void CreateDevice();
	void CreateBuffers();
	
	void CreateRasterizerState();
	void CreateDepthBuffer();
	void CreateDepthStencilState();
	//void CreateNoDepthStencilState();

public:
	template <typename T_VERTEX>
	void CreateVertexBuffer(const vector<T_VERTEX>& vertices, ComPtr<ID3D11Buffer>& vertexBuffer)
	{
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
		bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
		bufferDesc.StructureByteStride = sizeof(T_VERTEX);

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 }; // MS 예제에서 초기화하는 방식
		vertexBufferData.pSysMem = vertices.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		const HRESULT hr =
			Device->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
		if (FAILED(hr)) {
			std::cout << "CreateBuffer() failed. " << std::hex << hr << std::endl;
		};
	}

	template <typename T_CONSTANT>
	void CreateConstantBuffer(const T_CONSTANT& constantBufferData, ComPtr<ID3D11Buffer>& constantBuffer) 
	{
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(constantBufferData);
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		// Fill in the subresource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &constantBufferData;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		Device->CreateBuffer(&cbDesc, &InitData, constantBuffer.GetAddressOf());
	}

	template <typename T_DATA>
	void UpdateBuffer(const T_DATA& bufferData, ComPtr<ID3D11Buffer>& buffer) 
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		DeviceContext->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, &bufferData, sizeof(bufferData));
		DeviceContext->Unmap(buffer.Get(), NULL);
	}

	void CreateIndexBuffer(const std::vector<uint16_t>& indices, ComPtr<ID3D11Buffer>& m_indexBuffer);
	void CreateIndexBuffer(const std::vector<uint32_t>& indices, ComPtr<ID3D11Buffer>& m_indexBuffer);

public:
	ID3D11Device* GetDevice() { return Device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return DeviceContext.Get(); }
	ComPtr<ID3D11Device> GetDeviceCom() const { return Device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContextCom() const { return DeviceContext; }

	ComPtr<ID3D11RenderTargetView> GetMainRenderTargetView();
	ComPtr<ID3D11ShaderResourceView> GetMainShaderResourceView();

	void StartRenderPass();

	void Present();

	void ResizeScreen(float InWidth, float InHeight);

public:
	ID3D11Texture2D* GetRTVTexture();

public:
	void CreateVertexShaderAndInputLayout(const wstring& filename,
		const vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
		ComPtr<ID3D11VertexShader>& vertexShader,
		ComPtr<ID3D11InputLayout>& inputLayout);
	void CreateHullShader(const wstring& filename, ComPtr<ID3D11HullShader>& hullShader);
	void CreateDomainShader(const wstring& filename, ComPtr<ID3D11DomainShader>& domainShdaer);
	void CreatePixelShader(const wstring& filename, ComPtr<ID3D11PixelShader>& pixelShader);
	void CreateGeometryShader(const wstring& filename, ComPtr<ID3D11GeometryShader>& geometeyShader);

public:
	void CreateTexture(const string filename, const bool useSRGB, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& textureResourceView);
	void CreateTextureArray(vector<string> filenames, const bool useSRGB, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& textureResourceView);
	void CreateDDSTexture(const wchar_t* filename, bool isCubeMap, ComPtr<ID3D11ShaderResourceView>& textureResourceView);
	void CreateMetallicRoughnessTexture(string metallicFilename, const string roughnessFilename, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& srv);

public:
	FDynamicMulticastDelegate OnReSizeDelegate;

private:
	D3D();
	~D3D();

public:
	void Init();
	void UpdateGUI();
	void Render();

private:
	static D3D* Instance;

private:
	static D3DDesc D3dDesc;

private:
	UINT m_numQualityLevels = 0;
	bool m_useMSAA = true;

private:
	ComPtr<IDXGISwapChain> SwapChain;

	ComPtr<ID3D11Device> Device;
	ComPtr<ID3D11DeviceContext> DeviceContext;

	ComPtr<ID3D11RenderTargetView> m_backBufferRTV;
	ComPtr<ID3D11ShaderResourceView> ShaderResourceView;

	ComPtr<ID3D11RasterizerState> RasterizerState;

	ComPtr<ID3D11Texture2D> DSV_Texture;
	ComPtr<ID3D11DepthStencilView> DepthStencilView;
	ComPtr<ID3D11DepthStencilState> DepthStencilState;

private:
	ComPtr<ID3D11Texture2D> m_floatBuffer;
	ComPtr<ID3D11ShaderResourceView> m_floatSRV;
	ComPtr<ID3D11RenderTargetView> m_floatRTV;

private:
	ComPtr<ID3D11Texture2D> m_resolvedBuffer;
	ComPtr<ID3D11RenderTargetView> m_resolvedRTV;
	ComPtr<ID3D11ShaderResourceView> m_resolvedSRV;

private:
	PostProcess m_postProcess;

};