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
	float GetBloomStrength() { return m_postProcess.m_combineFilter.m_constData.strength; }
	float GetExposure() { return m_postProcess.m_combineFilter.m_constData.option1; }
	float GetGamma() { return m_postProcess.m_combineFilter.m_constData.option2; }
	int GetFilter() { return m_postProcess.m_combineFilter.m_constData.filterType; }
	float GetDepthScale() { return m_postEffectsConstsCPU.depthScale; }
	float GetFogStrength() { return m_postEffectsConstsCPU.fogStrength; }
	int GetPostFxMode() { return m_postEffectsConstsCPU.mode; }
	bool GetWireRendering() { return bWireRender; }

	void SetBloomStrength(float InValue) { m_postProcess.m_combineFilter.m_constData.strength = InValue; }
	void SetExposure(float InValue){ m_postProcess.m_combineFilter.m_constData.option1 = InValue;}
	void SetGamma(float InValue) { m_postProcess.m_combineFilter.m_constData.option2 = InValue;}
	void SetFilter(int InValue) { m_postProcess.m_combineFilter.m_constData.filterType = InValue; }
	void SetDepthScale(float InValue) { m_postEffectsConstsCPU.depthScale = InValue;}
	void SetFogStrength(float InValue) { m_postEffectsConstsCPU.fogStrength = InValue;}
	void SetPostFxMode(int InValue) { m_postEffectsConstsCPU.mode = InValue;}
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
	PostEffects m_postEffects;
	PostProcess m_postProcess;

private:
	D3D11_VIEWPORT Viewport;

private:
	// 공통으로 사용하는 텍스처들
	ComPtr<ID3D11ShaderResourceView> m_envSRV;
	ComPtr<ID3D11ShaderResourceView> m_specularSRV;
	ComPtr<ID3D11ShaderResourceView> m_irradianceSRV;
	ComPtr<ID3D11ShaderResourceView> m_brdfSRV;

private:
	// 공통으로 사용하는 상수 데이터	
	GlobalConstants m_globalConstsCPU;
	GlobalConstants m_reflectGlobalConstsCPU;
	GlobalConstants m_shadowGlobalConstsCPU[MAX_LIGHTS];
	PostEffectsConstants m_postEffectsConstsCPU;

	ComPtr<ID3D11Buffer> m_globalConstsGPU;
	ComPtr<ID3D11Buffer> m_reflectGlobalConstsGPU;
	ComPtr<ID3D11Buffer> m_shadowGlobalConstsGPU[MAX_LIGHTS];
	ComPtr<ID3D11Buffer> m_postEffectsConstsGPU;

private:
	// PostProcess
	ComPtr<ID3D11Texture2D> m_floatBuffer;
	ComPtr<ID3D11ShaderResourceView> m_floatSRV;
	ComPtr<ID3D11RenderTargetView> m_floatRTV;

	ComPtr<ID3D11Texture2D> m_resolvedBuffer;
	ComPtr<ID3D11RenderTargetView> m_resolvedRTV;
	ComPtr<ID3D11ShaderResourceView> m_resolvedSRV;

	ComPtr<ID3D11Texture2D> m_postEffectsBuffer;
	ComPtr<ID3D11RenderTargetView> m_postEffectsRTV;
	ComPtr<ID3D11ShaderResourceView> m_postEffectsSRV;

	ComPtr<ID3D11Texture2D> Temp_DSV_Texture;
	ComPtr<ID3D11DepthStencilView> Temp_DepthStencilView;

	// ShadowMap
	int m_shadowWidth = 1280;
	int m_shadowHeight = 1280;
	ComPtr<ID3D11Texture2D> m_shadowBuffers[MAX_LIGHTS]; // No MSAA
	ComPtr<ID3D11DepthStencilView> m_shadowDSVs[MAX_LIGHTS];
	ComPtr<ID3D11ShaderResourceView> m_shadowSRVs[MAX_LIGHTS];

private:
	StructuredBuffer<InstanceData> m_instances;			// 공용 인스턴스 SB (SRV)
	UINT m_instanceCapacity = 0;						// 담을 수 있는 인스턴스 개수

	void EnsureInstanceCapacity(ID3D11Device* device, size_t needed);

private:
	StructuredBuffer<SkinnedInstanceDataGPU> m_skinnedInstances;	// transform, material 등
	UINT m_skinnedInstanceCapacity = 0;

	StructuredBuffer<Matrix> m_bonePalettes;					// 모든 인스턴스의 본 팔레트 모음
	UINT m_bonePaletteCapacity = 0;

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