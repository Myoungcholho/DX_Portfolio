#pragma once

class URenderer
{
public:
	URenderer();
	~URenderer();
	URenderer(const URenderer&) = delete;				// 복사 생성 금지
	URenderer& operator=(const URenderer&) = delete;	// 복사 대입 금지
	URenderer(URenderer&&) = delete;					// 이동 생성 금지
	URenderer& operator=(URenderer&&) = delete;			// 이동 대입 금지

public:
	void Init();
	void RenderFrame(const URenderQueue& queue);
	void UpdateGlobalConstants(
		const Vector3& eyeWorld,
		const Matrix& viewRow,
		const Matrix& projRow,
		const Matrix& refl = Matrix());
	void UpdateGlobalLights(const vector<LightData>& lights);

	void OnGUI() {}
	void RenderPostProcess();
	void Present();

public:
	float GetBloomStrength() { return postProcess.combineFilter.constData.strength; }
	float GetExposure() { return postProcess.combineFilter.constData.option1; }
	float GetGamma() { return postProcess.combineFilter.constData.option2; }
	int GetFilter() { return postProcess.combineFilter.constData.filterType; }
	float GetDepthScale() { return postEffectsConstsCPU.depthScale; }
	float GetFogStrength() { return postEffectsConstsCPU.fogStrength; }
	int GetPostFxMode() { return postEffectsConstsCPU.mode; }
	bool GetWireRendering() { return bWireRender; }

	void SetBloomStrength(float InValue) { postProcess.combineFilter.constData.strength = InValue; }
	void SetExposure(float InValue){ postProcess.combineFilter.constData.option1 = InValue;}
	void SetGamma(float InValue) { postProcess.combineFilter.constData.option2 = InValue;}
	void SetFilter(int InValue) { postProcess.combineFilter.constData.filterType = InValue; }
	void SetDepthScale(float InValue) { postEffectsConstsCPU.depthScale = InValue;}
	void SetFogStrength(float InValue) { postEffectsConstsCPU.fogStrength = InValue;}
	void SetPostFxMode(int InValue) { postEffectsConstsCPU.mode = InValue;}
	void SetWireRendering(int InValue) { bWireRender = InValue; }

	void SetGlobalConsts(ID3D11Buffer* globalConstsGPU);
private:
	void BindCommonResources();
	void BeginFrame();

	void RenderDepthOnly(const URenderQueue& queue);
	void RenderShadowMap(const URenderQueue& queue);

	void RenderOpaque(const URenderQueue& queue);
	void RenderSkinned(const URenderQueue& queue);
	void RenderSkyBox(const URenderQueue& queue);
	void RenderNormal(const URenderQueue& queue);
	void RenderMirror(const URenderQueue& queue);
	
	void EndFrame();

private:
	void RenderOpaqueInstanced(const URenderQueue& queue);
	void RenderSkinnedInstanced(const URenderQueue& queue);

private:
	void OnResize();

private:
	void SetShadowViewport();
	void BuildShadowGlobalConsts();

private:
	wstring path = L"../../../_CubeMapTexture/HDRI/FullSky/";
	wstring EnvHDR_Name = path + L"FullSkyEnvHDR.dds";
	wstring Specular_Name = path + L"FullSkySpecularHDR.dds";
	wstring DiffuseHDR_Name = path + L"FullSkyDiffuseHDR.dds";
	wstring Brdf_Name = path + L"FullSkyBrdf.dds";

private:
	PostEffects postEffects;
	PostProcess postProcess;

private:
	D3D11_VIEWPORT viewport;

private:
	// 공통으로 사용하는 텍스처들
	ComPtr<ID3D11ShaderResourceView> envSRV;
	ComPtr<ID3D11ShaderResourceView> specularSRV;
	ComPtr<ID3D11ShaderResourceView> irradianceSRV;
	ComPtr<ID3D11ShaderResourceView> brdfSRV;

private:
	// 공통으로 사용하는 상수 데이터	
	GlobalConstants globalConstsCPU;
	GlobalConstants reflectGlobalConstsCPU;
	GlobalConstants shadowGlobalConstsCPU[MAX_LIGHTS];
	PostEffectsConstants postEffectsConstsCPU;

	ComPtr<ID3D11Buffer> globalConstsGPU;
	ComPtr<ID3D11Buffer> reflectGlobalConstsGPU;
	ComPtr<ID3D11Buffer> shadowGlobalConstsGPU[MAX_LIGHTS];
	ComPtr<ID3D11Buffer> postEffectsConstsGPU;

private:
	// PostProcess
	ComPtr<ID3D11Texture2D> floatBuffer;
	ComPtr<ID3D11ShaderResourceView> floatSRV;
	ComPtr<ID3D11RenderTargetView> floatRTV;

	ComPtr<ID3D11Texture2D> resolvedBuffer;
	ComPtr<ID3D11RenderTargetView> resolvedRTV;
	ComPtr<ID3D11ShaderResourceView> resolvedSRV;

	ComPtr<ID3D11Texture2D> postEffectsBuffer;
	ComPtr<ID3D11RenderTargetView> postEffectsRTV;
	ComPtr<ID3D11ShaderResourceView> postEffectsSRV;

	ComPtr<ID3D11Texture2D> temp_DSV_Texture;
	ComPtr<ID3D11DepthStencilView> temp_DepthStencilView;

	// ShadowMap
	int shadowWidth = 1280;
	int shadowHeight = 1280;
	ComPtr<ID3D11Texture2D> shadowBuffers[MAX_LIGHTS]; // No MSAA
	ComPtr<ID3D11DepthStencilView> shadowDSVs[MAX_LIGHTS];
	ComPtr<ID3D11ShaderResourceView> shadowSRVs[MAX_LIGHTS];

private:
	StructuredBuffer<InstanceData> instances;			// 공용 인스턴스 SB (SRV)
	UINT instanceCapacity = 0;						// 담을 수 있는 인스턴스 개수

	void EnsureInstanceCapacity(ID3D11Device* device, size_t needed);

private:
	StructuredBuffer<SkinnedInstanceDataGPU> skinnedInstances;	// transform, material 등
	UINT skinnedInstanceCapacity = 0;

	StructuredBuffer<Matrix> bonePalettes;					// 모든 인스턴스의 본 팔레트 모음
	UINT bonePaletteCapacity = 0;

	void EnsureSkinnedInstanceCapacity(ID3D11Device* device, size_t needed);
	void EnsureBonePaletteCapacity(ID3D11Device* device, size_t needed);

	SkinnedBatchCB skinnedBatchConstsCPU;
	ComPtr<ID3D11Buffer> skinnedBatchConstsGPU;

private:
	ComPtr<ID3D11Device> device = nullptr;
	ComPtr<ID3D11DeviceContext> context = nullptr;
	UINT numQualityLevels = 0;
	bool useMSAA = true;

private:
	bool bWireRender = false;

private:
	FDelegateHandle ResizeHandle{};
};