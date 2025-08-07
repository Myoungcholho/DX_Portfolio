#include "Framework.h"
#include "URenderer.h"

URenderer::URenderer()
{
	device = D3D::Get()->GetDeviceCom();
	context = D3D::Get()->GetDeviceContextCom();
	numQualityLevels = D3D::Get()->m_numQualityLevels;
	useMSAA = D3D::Get()->m_useMSAA;

	float width = D3D::GetDesc().Width;
	float height = D3D::GetDesc().Height;

	Viewport = std::make_unique<D3D11_VIEWPORT>();
	Viewport->TopLeftX = 0;
	Viewport->TopLeftY = 0;
	Viewport->Width = width;
	Viewport->Height = height;
	Viewport->MinDepth = 0;
	Viewport->MaxDepth = 1;


	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = D3D::Get()->GetDesc().Width;
	desc.Height = D3D::Get()->GetDesc().Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// MSAA 설정
	if (useMSAA && numQualityLevels > 0)
	{
		desc.SampleDesc.Count = 4;
		desc.SampleDesc.Quality = numQualityLevels - 1;
	}
	else
	{
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
	}

	// 1. HDR용 Float RenderTarget
	device->CreateTexture2D(&desc, NULL, m_floatBuffer.GetAddressOf());
	device->CreateShaderResourceView(m_floatBuffer.Get(), NULL, m_floatSRV.GetAddressOf());
	device->CreateRenderTargetView(m_floatBuffer.Get(), NULL, m_floatRTV.GetAddressOf());

	// 2. Resolve용 단일샘플 텍스처
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	device->CreateTexture2D(&desc, NULL, m_resolvedBuffer.GetAddressOf());
	device->CreateShaderResourceView(m_resolvedBuffer.Get(), NULL, m_resolvedSRV.GetAddressOf());
	device->CreateRenderTargetView(m_resolvedBuffer.Get(), NULL, m_resolvedRTV.GetAddressOf());
}

void URenderer::Init()
{
	D3D11Utils::CreateConstBuffer(device, m_globalConstsCPU, m_globalConstsGPU);
	D3D11Utils::CreateConstBuffer(device, m_reflectGlobalConstsCPU, m_reflectGlobalConstsGPU);

	D3D11Utils::CreateDDSTexture(device, EnvHDR_Name.c_str(),true, m_envSRV);
	D3D11Utils::CreateDDSTexture(device, Specular_Name.c_str(), true, m_specularSRV);
	D3D11Utils::CreateDDSTexture(device, DiffuseHDR_Name.c_str(), true,m_irradianceSRV);
	D3D11Utils::CreateDDSTexture(device, Brdf_Name.c_str(),false, m_brdfSRV);

	m_postProcess.Initialize
	(	device, context, 
		{ m_resolvedSRV }, { D3D::Get()->GetBackBufferRTV() }, 
		D3D::Get()->GetDesc().Width, D3D::Get()->GetDesc().Height, 
		4
	);

}

//RenderLoop에서 호출
void URenderer::UpdateGlobalConstants(const Vector3& eyeWorld, const Matrix& viewRow, const Matrix& projRow, const Matrix& refl)
{
	m_globalConstsCPU.eyeWorld = eyeWorld;
	m_globalConstsCPU.view = viewRow.Transpose();
	m_globalConstsCPU.proj = projRow.Transpose();
	m_globalConstsCPU.viewProj = (viewRow * projRow).Transpose();

	m_reflectGlobalConstsCPU = m_globalConstsCPU;
	m_reflectGlobalConstsCPU.view = (refl * viewRow).Transpose();
	m_reflectGlobalConstsCPU.viewProj = (refl * viewRow * projRow).Transpose();

	D3D11Utils::UpdateBuffer(device, context, m_globalConstsCPU, m_globalConstsGPU);
	D3D11Utils::UpdateBuffer(device, context, m_reflectGlobalConstsCPU, m_reflectGlobalConstsGPU);
}

/// <summary>
/// 렌더링 전역 값 수정, URenderManager가 호출
/// </summary>
void URenderer::OnGUI()
{
	ImGui::Begin("URenderer");
	{
		ImGui::Text("-GlobalRenderOption-");
		ImGui::Checkbox("WireRendering", &bWireRender);

		int flag = 0;
		flag += ImGui::SliderFloat("Bloom Strength", &m_postProcess.m_combineFilter.m_constData.strength, 0.0f, 1.0f);
		flag += ImGui::SliderFloat("Exposure", &m_postProcess.m_combineFilter.m_constData.option1,0.0f, 10.0f);
		flag += ImGui::SliderFloat("Gamma", &m_postProcess.m_combineFilter.m_constData.option2, 0.1f, 5.0f);

		// 여기 값복사만 해야한다.
		if(flag)
			m_postProcess.m_combineFilter.UpdateConstantBuffers(device, context);
	}
	ImGui::End();
}

void URenderer::RenderFrame(const URenderQueue& queue)
{
	BeginFrame();

	RenderSkyBox(queue);
	RenderOpaque(queue);

	EndFrame();
}

/// <summary>
/// 하는 일 요약
/// 1. ClearRenderTarget, ClearDepthStencil
/// 2. OMSetRenderTarget
/// 3. 공용 Samplers 바인딩
/// 4. 공용 SRV 바인딩
/// 5. 공용 Constant 바인딩
/// </summary>
void URenderer::BeginFrame()
{
	// ClearRTV,DSV
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	vector<ID3D11RenderTargetView*> rtvs = { m_floatRTV.Get() };
	for (size_t i = 0; i < rtvs.size(); ++i)
	{
		context->ClearRenderTargetView(rtvs[i], clearColor);
	}

	context->OMSetRenderTargets(UINT(rtvs.size()), rtvs.data(), D3D::Get()->GetDepthStencilView().Get());

	context->ClearDepthStencilView(D3D::Get()->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 모든 샘플러를 공통으로 사용
	context->VSSetSamplers(0, UINT(Graphics::sampleStates.size()), Graphics::sampleStates.data());
	context->PSSetSamplers(0, UINT(Graphics::sampleStates.size()), Graphics::sampleStates.data());

	// 공통 텍스처 상수버퍼로 Push
	vector<ID3D11ShaderResourceView*> commonSRVs =
	{
		m_envSRV.Get(),
		m_specularSRV.Get(),
		m_irradianceSRV.Get(),
		m_brdfSRV.Get()
	};
	context->PSSetShaderResources(10, UINT(commonSRVs.size()), commonSRVs.data());

	// 공통 상수 버퍼 업데이트
	context->VSSetConstantBuffers(1, 1, m_globalConstsGPU.GetAddressOf());
	context->PSSetConstantBuffers(1, 1, m_globalConstsGPU.GetAddressOf());
	context->GSSetConstantBuffers(1, 1, m_globalConstsGPU.GetAddressOf());

	// Viewport설정
	D3D::Get()->GetDeviceContext()->RSSetViewports(1, Viewport.get());
}

void URenderer::RenderSkyBox(const URenderQueue& queue)
{
	bWireRender ? Graphics::skyboxWirePSO.Apply(context.Get()) : Graphics::skyboxSolidPSO.Apply(context.Get());

	for (auto* comp : queue.GetSkyboxList())
	{
		comp->UpdateConstantBuffers(device, context);
		comp->Draw(context.Get());
	}
}

void URenderer::RenderOpaque(const URenderQueue& queue)
{
	bWireRender ? Graphics::defaultWirePSO.Apply(context.Get()) : Graphics::defaultSolidPSO.Apply(context.Get());

	for (auto* comp : queue.GetOpaqueList())
	{
		comp->UpdateConstantBuffers(device, context);
		comp->Draw(context.Get());
	}
}


void URenderer::RenderMirror(const URenderQueue& queue)
{

}

void URenderer::EndFrame()
{
	
}

void URenderer::RenderPostProcess()
{
	const float clearColor[4] = { 0, 0, 0, 1 };

	// Clear 백버퍼 (안 하면 겹침 생길 수 있음)
	context->ClearRenderTargetView(D3D::Get()->GetBackBufferRTV().Get(), clearColor);

	context->ResolveSubresource
	(
		m_resolvedBuffer.Get(), // 대상 (싱글샘플 텍스처)
		0,                      // 대상 서브리소스 index
		m_floatBuffer.Get(),    // 소스 (MSAA 텍스처)
		0,                      // 소스 서브리소스 index
		DXGI_FORMAT_R16G16B16A16_FLOAT // 포맷 일치해야 함
	);

	Graphics::postProcessingPSO.Apply(context.Get());
	m_postProcess.Render(context);
}

void URenderer::Present()
{
	//ImGuiManager::Get()->RenderStoredDrawData();
	D3D::Get()->Present();
}