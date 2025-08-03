#pragma once

class URenderer
{
public:
	URenderer();

public:
	void Init();
	void RenderFrame(URenderQueue* queue);
	void UpdateGlobalConstants(
		const Vector3& eyeWorld,
		const Matrix& viewRow,
		const Matrix& projRow,
		const Matrix& refl = Matrix());
	void OnGUI();
	void RenderPostProcess();
	void Present();

private:
	void BeginFrame();
	void RenderOpaque(URenderQueue* queue);
	void RenderSkyBox(URenderQueue* queue);
	void RenderMirror(URenderQueue* queue);
	//void RenderPostProcess(URenderQueue* queue);
	void EndFrame();


private:
	wstring path = L"../../../_CubeMapTexture/HDRI/MorningSkyHalf/";
	wstring EnvHDR_Name = path + L"MorningSkyHalfCubeEnvHDR.dds";
	wstring Specular_Name = path + L"MorningSkyHalfCubeSpecularHDR.dds";
	wstring DiffuseHDR_Name = path + L"MorningSkyHalfCubeDiffuseHDR.dds";
	wstring Brdf_Name = path + L"MorningSkyHalfCubeBrdf.dds";

private:
	PostProcess m_postProcess;

private:
	// 공통으로 사용하는 텍스처들
	ComPtr<ID3D11ShaderResourceView> m_envSRV;
	ComPtr<ID3D11ShaderResourceView> m_specularSRV;
	ComPtr<ID3D11ShaderResourceView> m_irradianceSRV;
	ComPtr<ID3D11ShaderResourceView> m_brdfSRV;

	// 공통으로 사용하는 상수 데이터
	GlobalConstants m_globalConstsCPU;
	GlobalConstants m_reflectGlobalConstsCPU;
	ComPtr<ID3D11Buffer> m_globalConstsGPU;
	ComPtr<ID3D11Buffer> m_reflectGlobalConstsGPU;


private:
	// PostProcess
	ComPtr<ID3D11Texture2D> m_floatBuffer;
	ComPtr<ID3D11ShaderResourceView> m_floatSRV;
	ComPtr<ID3D11RenderTargetView> m_floatRTV;

	ComPtr<ID3D11Texture2D> m_resolvedBuffer;
	ComPtr<ID3D11RenderTargetView> m_resolvedRTV;
	ComPtr<ID3D11ShaderResourceView> m_resolvedSRV;

	ComPtr<ID3D11Texture2D> Temp_DSV_Texture;
	ComPtr<ID3D11DepthStencilView> Temp_DepthStencilView;

private:
	ComPtr<ID3D11Device> device = nullptr;
	ComPtr<ID3D11DeviceContext> context = nullptr;
	UINT numQualityLevels = 0;
	bool useMSAA = true;

private:
	bool bWireRender = false;
};