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

	// 3. PostEffect용
	device->CreateTexture2D(&desc, NULL, m_postEffectsBuffer.GetAddressOf());
	device->CreateShaderResourceView(m_postEffectsBuffer.Get(), NULL, m_postEffectsSRV.GetAddressOf());
	device->CreateRenderTargetView(m_postEffectsBuffer.Get(), NULL, m_postEffectsRTV.GetAddressOf());

	// 4. Shadow용
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.Width = m_shadowWidth;
	desc.Height = m_shadowHeight;
	for (int i = 0; i < MAX_LIGHTS; ++i)
		device->CreateTexture2D(&desc, NULL, m_shadowBuffers[i].GetAddressOf());
	
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	
	for (int i = 0; i < MAX_LIGHTS; ++i)
		device->CreateDepthStencilView(m_shadowBuffers[i].Get(), &dsvDesc, m_shadowDSVs[i].GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < MAX_LIGHTS; ++i)
		device->CreateShaderResourceView(m_shadowBuffers[i].Get(), &srvDesc, m_shadowSRVs[i].GetAddressOf());
}

URenderer::~URenderer()
{
	if (m_resizeHandle.IsValid()) {
		D3D::Get()->OnReSizeDelegate.Remove(m_resizeHandle);
		m_resizeHandle = FDelegateHandle{};
	}
}

void URenderer::Init()
{
	D3D11Utils::CreateConstBuffer(device, m_globalConstsCPU, m_globalConstsGPU);
	D3D11Utils::CreateConstBuffer(device, m_reflectGlobalConstsCPU, m_reflectGlobalConstsGPU);
	D3D11Utils::CreateConstBuffer(device, m_postEffectsConstsCPU, m_postEffectsConstsGPU);
	for (int i = 0; i < MAX_LIGHTS; i++) {
		D3D11Utils::CreateConstBuffer(device, m_shadowGlobalConstsCPU[i], m_shadowGlobalConstsGPU[i]);
	}


	D3D11Utils::CreateDDSTexture(device, EnvHDR_Name.c_str(),true, m_envSRV);
	D3D11Utils::CreateDDSTexture(device, Specular_Name.c_str(), true, m_specularSRV);
	D3D11Utils::CreateDDSTexture(device, DiffuseHDR_Name.c_str(), true,m_irradianceSRV);
	D3D11Utils::CreateDDSTexture(device, Brdf_Name.c_str(),false, m_brdfSRV);

	m_postEffects.Initialize
	(
		device, context
	);

	m_postProcess.Initialize
	(	device, context, 
		{ m_postEffectsSRV }, { D3D::Get()->GetFinalLDR_RTV() },
		D3D::Get()->GetDesc().Width, D3D::Get()->GetDesc().Height, 
		4
	);

	m_resizeHandle = D3D::Get()->OnReSizeDelegate.AddDynamic(this, &URenderer::OnResize, "URenderer::OnResize");
}

//RenderLoop에서 호출
void URenderer::UpdateGlobalLights(const vector<LightData>& lights)
{
	for (int i = 0; i < lights.size(); ++i)
	{
		if (i >= MAX_LIGHTS)
			break;
		m_globalConstsCPU.lights[i] = lights[i];
	}
}

void URenderer::UpdateGlobalConstants(const Vector3& eyeWorld, const Matrix& viewRow, const Matrix& projRow, const Matrix& refl)
{
	m_globalConstsCPU.eyeWorld = eyeWorld;
	m_globalConstsCPU.view = viewRow.Transpose();
	m_globalConstsCPU.proj = projRow.Transpose();
	m_globalConstsCPU.invProj = projRow.Invert().Transpose();
	m_globalConstsCPU.viewProj = (viewRow * projRow).Transpose();
	m_globalConstsCPU.invViewProj = m_globalConstsCPU.viewProj.Invert();

	m_reflectGlobalConstsCPU = m_globalConstsCPU;
	m_reflectGlobalConstsCPU.view = (refl * viewRow).Transpose();
	m_reflectGlobalConstsCPU.viewProj = (refl * viewRow * projRow).Transpose();

	D3D11Utils::UpdateBuffer(device, context, m_globalConstsCPU, m_globalConstsGPU);
	D3D11Utils::UpdateBuffer(device, context, m_reflectGlobalConstsCPU, m_reflectGlobalConstsGPU);
}


void URenderer::SetGlobalConsts(ComPtr<ID3D11Buffer>& globalConstsGPU)
{
	context->VSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf());
	context->PSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf());
	context->GSSetConstantBuffers(1, 1, globalConstsGPU.GetAddressOf());
}

void URenderer::RenderFrame(const URenderQueue& queue)
{
	BindCommonResources();					// Viewport, Sampler, 공통 상수 바인딩

	RenderDepthOnly(queue);					// 안개처리용 DepthMap 구성
	RenderShadowMap(queue);					// 그림자용 ShadowMap 구성

	BeginFrame();							// MainPASS) OM 바인딩

	RenderSkyBox(queue);					// MainPASS) 물체렌더링
	RenderOpaque(queue);					// MainPASS) 물체렌더링
	RenderNormal(queue);					// MainPASS) 물체렌더링


	EndFrame();								// 현재 작업 X
}


void URenderer::BindCommonResources()
{
	// Viewport설정
	D3D::Get()->GetDeviceContext()->RSSetViewports(1, Viewport.get());

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
}

/// <summary>
/// 깊이 맵 생성 [안개]
/// </summary>
void URenderer::RenderDepthOnly(const URenderQueue& queue)
{
	SetGlobalConsts(m_globalConstsGPU);

	context->OMSetRenderTargets(0, NULL ,D3D::Get()->GetDepthOnly_DSV().Get());
	context->ClearDepthStencilView(D3D::Get()->GetDepthOnly_DSV().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	Graphics::depthOnlyPSO.Apply(context.Get());

	for (auto* comp : queue.GetOpaqueList())
	{
		comp->UpdateConstantBuffers(device, context);
		comp->Draw(context.Get());
	}

	for (auto* comp : queue.GetSkyboxList())
	{
		comp->UpdateConstantBuffers(device, context);
		comp->Draw(context.Get());
	}
}

/// <summary>
/// 그림자 맵 생성
/// </summary>
void URenderer::RenderShadowMap(const URenderQueue& queue)
{
	SetShadowViewport();													// 그림자용 viewport 변경

	BuildShadowGlobalConsts();

	Graphics::depthOnlyPSO.Apply(context.Get());

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (m_globalConstsCPU.lights[i].type & LIGHT_SHADOW)
		{
			context->OMSetRenderTargets(0, NULL, m_shadowDSVs[i].Get());
			context->ClearDepthStencilView(m_shadowDSVs[i].Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

			SetGlobalConsts(m_shadowGlobalConstsGPU[i]);
			
			for (URenderProxy* comp : queue.GetOpaqueList())
			{
				if (comp->bVisible && comp->m_castShadow)
				{
					comp->UpdateConstantBuffers(device, context);
					comp->Draw(context.Get());
				}
			}

			for (auto* comp : queue.GetSkyboxList())
			{
				comp->UpdateConstantBuffers(device, context);
				comp->Draw(context.Get());
			}
		}
	}

	D3D::Get()->GetDeviceContext()->RSSetViewports(1, Viewport.get());		// viewport 복구
}

void URenderer::BeginFrame()
{
	// 메인 렌더링 셋팅
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	vector<ID3D11RenderTargetView*> rtvs = { m_floatRTV.Get() };

	for (size_t i = 0; i < rtvs.size(); ++i)
	{
		context->ClearRenderTargetView(rtvs[i], clearColor);
	}

	context->OMSetRenderTargets(UINT(rtvs.size()), rtvs.data(), D3D::Get()->GetDepthStencilView().Get());
	context->ClearDepthStencilView(D3D::Get()->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	// ShadowMap 전달
	vector<ID3D11ShaderResourceView*> shadowSRVs;
	for (int i = 0; i < MAX_LIGHTS; ++i)
		shadowSRVs.push_back(m_shadowSRVs[i].Get());
	context->PSSetShaderResources(15, UINT(shadowSRVs.size()), shadowSRVs.data());

	SetGlobalConsts(m_globalConstsGPU);
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

void URenderer::RenderNormal(const URenderQueue& queue)
{
	Graphics::normalsPSO.Apply(context.Get());
	for (auto* comp : queue.GetOpaqueList())
	{
		if (comp->m_drawNormal)
			comp->DrawNormal(context.Get());
	}

}

void URenderer::RenderMirror(const URenderQueue& queue)
{

}

void URenderer::EndFrame()
{
	
}

void URenderer::OnResize()
{
	numQualityLevels = D3D::Get()->m_numQualityLevels;
	useMSAA = D3D::Get()->m_useMSAA;

	// width, height 변경된거 재설정하고
	// QualityLevels가 1이상이라면 Count는 4로 고정사용중
	Viewport->TopLeftX = 0; Viewport->TopLeftY = 0;
	Viewport->Width = float(D3D::Get()->GetDesc().Width);
	Viewport->Height = float(D3D::Get()->GetDesc().Height);
	Viewport->MinDepth = 0.0f; Viewport->MaxDepth = 1.0f;
	context->RSSetViewports(1, Viewport.get());

	m_floatSRV.Reset();   m_floatRTV.Reset();   m_floatBuffer.Reset();
	m_resolvedSRV.Reset(); m_resolvedRTV.Reset(); m_resolvedBuffer.Reset();
	m_postEffectsSRV.Reset(); m_postEffectsRTV.Reset(); m_postEffectsBuffer.Reset();
	// postprocess resize 호출

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = D3D::Get()->GetDesc().Width; desc.Height = D3D::Get()->GetDesc().Height;
	desc.MipLevels = 1; desc.ArraySize = 1;
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

	// 1. HDR 재생성
	device->CreateTexture2D(&desc, nullptr, m_floatBuffer.GetAddressOf());
	device->CreateRenderTargetView(m_floatBuffer.Get(), nullptr, m_floatRTV.GetAddressOf());
	device->CreateShaderResourceView(m_floatBuffer.Get(), nullptr, m_floatSRV.GetAddressOf());

	// 2. 리졸브용
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	device->CreateTexture2D(&desc, nullptr, m_resolvedBuffer.GetAddressOf());
	device->CreateRenderTargetView(m_resolvedBuffer.Get(), nullptr, m_resolvedRTV.GetAddressOf());
	device->CreateShaderResourceView(m_resolvedBuffer.Get(), nullptr, m_resolvedSRV.GetAddressOf());

	// 3. posteffects용
	device->CreateTexture2D(&desc, nullptr, m_postEffectsBuffer.GetAddressOf());
	device->CreateRenderTargetView(m_postEffectsBuffer.Get(), nullptr, m_postEffectsRTV.GetAddressOf());
	device->CreateShaderResourceView(m_postEffectsBuffer.Get(), nullptr, m_postEffectsSRV.GetAddressOf());

	// 포스트프로세스 재설정
	m_postProcess.OnResize
	(device, context,
		{ m_postEffectsSRV }, { D3D::Get()->GetFinalLDR_RTV() },
		D3D::Get()->GetDesc().Width, D3D::Get()->GetDesc().Height,
		4
	);

}

void URenderer::SetShadowViewport()
{
	D3D11_VIEWPORT shadowViewport;
	ZeroMemory(&shadowViewport, sizeof(D3D11_VIEWPORT));
	shadowViewport.TopLeftX = 0;
	shadowViewport.TopLeftY = 0;
	shadowViewport.Width = float(m_shadowWidth);
	shadowViewport.Height = float(m_shadowHeight);
	shadowViewport.MinDepth = 0.0f;
	shadowViewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &shadowViewport);
}

/// <summary>
/// LightData들로 Light의 View, Proj데이터 만들어 저장
/// </summary>
void URenderer::BuildShadowGlobalConsts()
{
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		const LightData& light = m_globalConstsCPU.lights[i];

		if ((light.type & LIGHT_SHADOW) == false)
			continue;

		Vector3 up(0.f, 1.f, 0.f);

		float dot = up.Dot(light.direction);

		if (fabs(dot) > 0.99f)
			up = Vector3(1.0f, 0.0f, 0.0f);

		Matrix lightViewRow = XMMatrixLookAtLH(light.position, light.position + light.direction, up);
		Matrix lightProjRow = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), 1.0f, 0.01f, 100.0f);

		m_shadowGlobalConstsCPU[i].eyeWorld = light.position;
		m_shadowGlobalConstsCPU[i].view = lightViewRow.Transpose();
		m_shadowGlobalConstsCPU[i].proj = lightProjRow.Transpose();
		m_shadowGlobalConstsCPU[i].invProj = lightProjRow.Invert().Transpose();
		m_shadowGlobalConstsCPU[i].viewProj = (lightViewRow * lightProjRow).Transpose();

		m_globalConstsCPU.lights[i].viewProj = m_shadowGlobalConstsCPU[i].viewProj;
		m_globalConstsCPU.lights[i].invProj = m_shadowGlobalConstsCPU[i].invProj;

		D3D11Utils::UpdateBuffer(device, context, m_shadowGlobalConstsCPU[i], m_shadowGlobalConstsGPU[i]);
	}
	D3D11Utils::UpdateBuffer(device, context, m_globalConstsCPU, m_globalConstsGPU);
}

void URenderer::RenderPostProcess()
{
	const float clearColor[4] = { 0, 0, 0, 1 };

	// Clear 백버퍼 (안 하면 겹침 생길 수 있음)
	context->ClearRenderTargetView(D3D::Get()->GetBackBufferRTV().Get(), clearColor);

	context->ResolveSubresource
	(
		m_resolvedBuffer.Get(),				 // 대상 (싱글샘플 텍스처)
		0,									 // 대상 서브리소스 index
		m_floatBuffer.Get(),				 // 소스 (MSAA 텍스처)
		0,									 // 소스 서브리소스 index
		DXGI_FORMAT_R16G16B16A16_FLOAT		 // 포맷 일치해야 함
	);

	// PostEffects
	Graphics::postEffectsPSO.Apply(context.Get());
	D3D11Utils::UpdateBuffer(device,context, m_postEffectsConstsCPU, m_postEffectsConstsGPU);

	// ----------------그림자맵 확인용------------------------
	//SetGlobalConsts(m_shadowGlobalConstsGPU[0]);
	//vector<ID3D11ShaderResourceView*> postEffectsSRVs = 
	//{
	//	m_resolvedSRV.Get(), m_shadowSRVs[0].Get() 
	//};
	//context->PSSetShaderResources(20, UINT(postEffectsSRVs.size()),postEffectsSRVs.data());
	//context->OMSetRenderTargets(1, m_postEffectsRTV.GetAddressOf(), NULL);
	//context->PSSetConstantBuffers(3, 1,m_postEffectsConstsGPU.GetAddressOf());
	//m_postEffects.Render(context);

	// -------------------------------------------------------

	vector<ID3D11ShaderResourceView*> postEffectsSRVs =
	{
		m_resolvedSRV.Get(), D3D::Get()->GetDepthOnly_SRV().Get()
	};

	context->PSSetShaderResources(20, UINT(postEffectsSRVs.size()), postEffectsSRVs.data());

	context->OMSetRenderTargets(1, m_postEffectsRTV.GetAddressOf(), NULL);
	context->PSSetConstantBuffers(3, 1, m_postEffectsConstsGPU.GetAddressOf());
	m_postEffects.Render(context);

	// -------------------------------------------------------

	// PostProcess
	Graphics::postProcessingPSO.Apply(context.Get());

	m_postProcess.m_combineFilter.UpdateConstantBuffers(device, context);
	m_postProcess.Render(context);
}

void URenderer::Present()
{
	D3D::Get()->Present();
}