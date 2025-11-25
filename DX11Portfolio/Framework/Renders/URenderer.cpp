#include "Framework.h"
#include "URenderer.h"

static size_t NextPow2(size_t v) 
{
	size_t p = 1;
	while (p < v) p <<= 1;
	return p;
}

URenderer::URenderer()
{
	device = D3D::Get()->GetDeviceCom();
	context = D3D::Get()->GetDeviceContextCom();
	numQualityLevels = D3D::Get()->numQualityLevels;
	useMSAA = D3D::Get()->useMSAA;

	float width = D3D::GetDesc().Width;
	float height = D3D::GetDesc().Height;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;


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
	ThrowIfFailed(device->CreateTexture2D(&desc, NULL, floatBuffer.GetAddressOf()));
	ThrowIfFailed(device->CreateShaderResourceView(floatBuffer.Get(), NULL, floatSRV.GetAddressOf()));
	ThrowIfFailed(device->CreateRenderTargetView(floatBuffer.Get(), NULL, floatRTV.GetAddressOf()));

	// 2. Resolve용 단일샘플 텍스처
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	ThrowIfFailed(device->CreateTexture2D(&desc, NULL, resolvedBuffer.GetAddressOf()));
	ThrowIfFailed(device->CreateShaderResourceView(resolvedBuffer.Get(), NULL, resolvedSRV.GetAddressOf()));
	ThrowIfFailed(device->CreateRenderTargetView(resolvedBuffer.Get(), NULL, resolvedRTV.GetAddressOf()));

	// 3. PostEffect용
	ThrowIfFailed(device->CreateTexture2D(&desc, NULL, postEffectsBuffer.GetAddressOf()));
	ThrowIfFailed(device->CreateShaderResourceView(postEffectsBuffer.Get(), NULL, postEffectsSRV.GetAddressOf()));
	ThrowIfFailed(device->CreateRenderTargetView(postEffectsBuffer.Get(), NULL, postEffectsRTV.GetAddressOf()));

	// 4. Shadow용
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.Width = shadowWidth;
	desc.Height = shadowHeight;
	for (int i = 0; i < MAX_LIGHTS; ++i)
		ThrowIfFailed(device->CreateTexture2D(&desc, NULL, shadowBuffers[i].GetAddressOf()));
	
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	
	for (int i = 0; i < MAX_LIGHTS; ++i)
		ThrowIfFailed(device->CreateDepthStencilView(shadowBuffers[i].Get(), &dsvDesc, shadowDSVs[i].GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < MAX_LIGHTS; ++i)
		ThrowIfFailed(device->CreateShaderResourceView(shadowBuffers[i].Get(), &srvDesc, shadowSRVs[i].GetAddressOf()));

	// SkinnedInstance용
	D3D11Utils::CreateConstBuffer(device, skinnedBatchConstsCPU, skinnedBatchConstsGPU);
}

URenderer::~URenderer()
{
	if (ResizeHandle.IsValid()) 
	{
		D3D::Get()->OnReSizeDelegate.Remove(ResizeHandle);
		ResizeHandle = FDelegateHandle{};
	}
}

/// <summary>
/// 글로벌 GPU버퍼, 포스트이펙트 GPU버퍼, 쉐도우GPU버퍼, 배경으로 사용될 텍스처를 만들고
/// 포스트프로세스를 초기화하는 작업을 실행
/// Resize델리게이트도 연결
/// </summary>
void URenderer::Init()
{
	D3D11Utils::CreateConstBuffer(device, globalConstsCPU, globalConstsGPU);
	D3D11Utils::CreateConstBuffer(device, reflectGlobalConstsCPU, reflectGlobalConstsGPU);
	D3D11Utils::CreateConstBuffer(device, postEffectsConstsCPU, postEffectsConstsGPU);
	for (int i = 0; i < MAX_LIGHTS; i++) 
	{
		D3D11Utils::CreateConstBuffer(device, shadowGlobalConstsCPU[i], shadowGlobalConstsGPU[i]);
	}

	D3D11Utils::CreateDDSTexture(device, EnvHDR_Name.c_str(),true, envSRV);
	D3D11Utils::CreateDDSTexture(device, Specular_Name.c_str(), true, specularSRV);
	D3D11Utils::CreateDDSTexture(device, DiffuseHDR_Name.c_str(), true,irradianceSRV);
	D3D11Utils::CreateDDSTexture(device, Brdf_Name.c_str(),false, brdfSRV);

	postEffects.Initialize
	(
		device, context
	);

	postProcess.Initialize
	(	device, context, 
		{ postEffectsSRV }, { D3D::Get()->GetFinalLDR_RTV() },
		D3D::Get()->GetDesc().Width, D3D::Get()->GetDesc().Height, 
		4
	);

	ResizeHandle = D3D::Get()->OnReSizeDelegate.AddDynamic(this, &URenderer::OnResize, "URenderer::OnResize");
}

/// <summary>
/// 글로벌로 사용하는 Light의 CPU정보를 업데이트
/// </summary>
/// <param name="lights"></param>
void URenderer::UpdateGlobalLights(const vector<LightData>& lights)
{
	const size_t n = min(lights.size(), size_t(MAX_LIGHTS));

	for (size_t i = 0; i < n; ++i) 
		globalConstsCPU.lights[i] = lights[i];

	// 잔여 삭제
	for (size_t i = n; i < MAX_LIGHTS; ++i) 
		globalConstsCPU.lights[i] = {};
}

/// <summary>
/// 글로벌 eye, view, proj, invProj 등 렌더링 시 고유로 사용되는 것들을 업데이트
/// </summary>
/// <param name="eyeWorld"> 눈 위치 </param>
/// <param name="viewRow"> view 행렬 </param>
/// <param name="projRow"> proj 행렬 </param>
/// <param name="refl"> 반사 행렬 </param>
void URenderer::UpdateGlobalConstants(const Vector3& eyeWorld, const Matrix& viewRow, const Matrix& projRow, const Matrix& refl)
{
	globalConstsCPU.eyeWorld = eyeWorld;
	globalConstsCPU.view = viewRow.Transpose();
	globalConstsCPU.proj = projRow.Transpose();
	globalConstsCPU.invProj = projRow.Invert().Transpose();
	globalConstsCPU.viewProj = (viewRow * projRow).Transpose();
	globalConstsCPU.invViewProj = globalConstsCPU.viewProj.Invert();

	reflectGlobalConstsCPU = globalConstsCPU;
	reflectGlobalConstsCPU.view = (refl * viewRow).Transpose();
	reflectGlobalConstsCPU.viewProj = (refl * viewRow * projRow).Transpose();

	D3D11Utils::UpdateBuffer(device, context, globalConstsCPU, globalConstsGPU);
	D3D11Utils::UpdateBuffer(device, context, reflectGlobalConstsCPU, reflectGlobalConstsGPU);
}

/// <summary>
/// 글로벌로 사용할 GPU버퍼를 파이프라인에 바인딩
/// </summary>
/// <param name="globalConstsGPU"> 바인딩할 GPU버퍼 </param>
void URenderer::SetGlobalConsts(ID3D11Buffer* globalConstsGPU)
{
	context->VSSetConstantBuffers(1, 1, &globalConstsGPU);
	context->PSSetConstantBuffers(1, 1, &globalConstsGPU);
	context->GSSetConstantBuffers(1, 1, &globalConstsGPU);
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
	RenderSkinned(queue);					// MainPASS) 물체렌더링

	EndFrame();								// 현재 작업 X
}

/// <summary>
/// RSViewport, 샘플러, 배경Texture 등 모든 곳에서 사용되는 데이터 파이프라인에 바인딩
/// </summary>
void URenderer::BindCommonResources()
{
	// Viewport설정
	D3D::Get()->GetDeviceContext()->RSSetViewports(1, &viewport);

	// 모든 샘플러를 공통으로 사용
	context->VSSetSamplers(0, UINT(Graphics::sampleStates.size()), Graphics::sampleStates.data());
	context->PSSetSamplers(0, UINT(Graphics::sampleStates.size()), Graphics::sampleStates.data());

	// 공통 텍스처 상수버퍼로 Push
	vector<ID3D11ShaderResourceView*> commonSRVs =
	{
		envSRV.Get(),
		specularSRV.Get(),
		irradianceSRV.Get(),
		brdfSRV.Get()
	};
	context->PSSetShaderResources(10, UINT(commonSRVs.size()), commonSRVs.data());
}

/// <summary>
/// 깊이 맵 생성 [안개]
/// </summary>
void URenderer::RenderDepthOnly(const URenderQueue& queue)
{
	SetGlobalConsts(globalConstsGPU.Get());

	// Harzard 방지 PS에서 언바인딩
	D3D11Utils::UnbindIfBoundPS(context.Get(), D3D::Get()->GetDepthOnly_SRV().Get());

	context->OMSetRenderTargets(0, NULL ,D3D::Get()->GetDepthOnly_DSV().Get());
	context->ClearDepthStencilView(D3D::Get()->GetDepthOnly_DSV().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	Graphics::depthOnlyInstancePSO.Apply(context.Get());
	RenderOpaqueInstanced(queue);

	Graphics::depthOnlyPSO.Apply(context.Get());
	for (auto* comp : queue.GetSkyboxList())
	{
		comp->UpdateConstantBuffers(device, context);
		comp->Draw(context.Get());
	}
	
	Graphics::depthOnlySkinnedInstancePSO.Apply(context.Get());
	RenderSkinnedInstanced(queue);
}

/// <summary>
/// 그림자 맵 생성
/// </summary>
void URenderer::RenderShadowMap(const URenderQueue& queue)
{
	SetShadowViewport();													// 그림자용 viewport 변경

	BuildShadowGlobalConsts();												// Light용 view proj 데이터 저장

	D3D11Utils::UnbindPSRange(context.Get(), 15, MAX_LIGHTS);

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (globalConstsCPU.lights[i].type & LIGHT_SHADOW)
		{
			context->OMSetRenderTargets(0, NULL, shadowDSVs[i].Get());
			context->ClearDepthStencilView(shadowDSVs[i].Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

			SetGlobalConsts(shadowGlobalConstsGPU[i].Get());
			
			/*for (URenderProxy* comp : queue.GetOpaqueList())
			{
				if (comp->bVisible && comp->castShadow)
				{
					comp->UpdateConstantBuffers(device, context);
					comp->Draw(context.Get());
				}
			}*/

			Graphics::depthOnlyInstancePSO.Apply(context.Get());
			RenderOpaqueInstanced(queue);

			Graphics::depthOnlyPSO.Apply(context.Get());

			for (URenderProxy* comp : queue.GetSkyboxList())
			{
				comp->UpdateConstantBuffers(device, context);
				comp->Draw(context.Get());
			}

			/*Graphics::depthOnlySkinnedPSO.Apply(context.Get());
			for (URenderProxy* comp : queue.GetSkinnedList())
			{
				comp->UpdateConstantBuffers(device, context);
				comp->Draw(context.Get());
			}*/

			Graphics::depthOnlySkinnedInstancePSO.Apply(context.Get());
			RenderSkinnedInstanced(queue);
		}
	}

	D3D::Get()->GetDeviceContext()->RSSetViewports(1, &viewport);		// viewport 복구
}

void URenderer::BeginFrame()
{
	// 메인 렌더링 셋팅
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	vector<ID3D11RenderTargetView*> rtvs = { floatRTV.Get() };

	for (size_t i = 0; i < rtvs.size(); ++i)
	{
		context->ClearRenderTargetView(rtvs[i], clearColor);
	}

	D3D11Utils::UnbindIfBoundPS(context.Get(), floatSRV.Get());

	context->OMSetRenderTargets(UINT(rtvs.size()), rtvs.data(), D3D::Get()->GetDepthStencilView().Get());
	context->ClearDepthStencilView(D3D::Get()->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//context->OMSetRenderTargets(UINT(rtvs.size()), rtvs.data(), D3D::Get()->GetDepthOnly_DSV().Get());
	
	// ShadowMap 전달
	vector<ID3D11ShaderResourceView*> shadowSRVsSnapShot;

	for (int i = 0; i < MAX_LIGHTS; ++i)
		shadowSRVsSnapShot.push_back(shadowSRVs[i].Get());

	context->PSSetShaderResources(15, UINT(shadowSRVsSnapShot.size()), shadowSRVsSnapShot.data());

	SetGlobalConsts(globalConstsGPU.Get());
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
	//bWireRender ? Graphics::defaultWirePSO.Apply(context.Get()) : Graphics::defaultSolidPSO.Apply(context.Get());

	/*for (auto* comp : queue.GetOpaqueList())
	{
		comp->UpdateConstantBuffers(device, context);
		comp->Draw(context.Get());
	}*/

	bWireRender ? Graphics::defaultWireInstancePSO.Apply(context.Get()) : Graphics::defaultSolidInstancePSO.Apply(context.Get());


	RenderOpaqueInstanced(queue);
}

void URenderer::RenderSkinned(const URenderQueue& queue)
{
	/*bWireRender ? Graphics::skinnedWirePSO.Apply(context.Get()) : Graphics::skinnedSolidPSO.Apply(context.Get());

	for (auto* comp : queue.GetSkinnedList())
	{
		comp->UpdateConstantBuffers(device, context);
		comp->Draw(context.Get());
	}*/

	bWireRender ? Graphics::skinnedWireInstancePSO.Apply(context.Get()) : Graphics::skinnedSolidInstancePSO.Apply(context.Get());

	RenderSkinnedInstanced(queue);
}

void URenderer::RenderNormal(const URenderQueue& queue)
{
	Graphics::normalsPSO.Apply(context.Get());
	for (auto* comp : queue.GetOpaqueList())
	{
		if (comp->drawNormal)
			comp->DrawNormal(context.Get());
	}

}

void URenderer::RenderMirror(const URenderQueue& queue)
{

}

void URenderer::EndFrame()
{
	
}

/// <summary>
/// Opaque 인스턴스들을 드로우콜을 실행하는 함수
/// </summary>
/// <param name="queue"></param>
void URenderer::RenderOpaqueInstanced(const URenderQueue& queue)
{
	const unordered_map<shared_ptr<const GPUMeshAsset>, InstanceBatch>& batches = queue.GetOpaqueInstanceBatches();

	// 물체들 순회
	for (unordered_map<shared_ptr<const GPUMeshAsset>, InstanceBatch>::const_iterator it = batches.begin(); it != batches.end(); ++it)
	{
		const shared_ptr<const GPUMeshAsset>& meshAsset = it->first;
		const InstanceBatch& batch = it->second;

		if (!meshAsset || batch.instances.empty())
			continue;

		// 1. 인스턴스 데이터 준비
		const size_t instanceCount = batch.instances.size();
		EnsureInstanceCapacity(device.Get(), instanceCount);

		// 2. CPU배열 채우기
		memcpy(instances.cpuData.data(), batch.instances.data(), instanceCount * sizeof(InstanceData));

		// 3. GPU 업로드
		instances.Upload(context.Get());

		// 4. 셰이더에 바인딩
		ID3D11ShaderResourceView* vsSrvs[1] = { instances.GetSRV() };
		context->VSSetShaderResources(8, 1, vsSrvs);
		context->PSSetShaderResources(8, 1, vsSrvs);

		// 5. 에셋의 서브 메쉬 순회하며 드로우
		for (const auto& mesh : meshAsset->meshes)
		{
			vector<ID3D11ShaderResourceView*> resViews =
			{
				mesh->albedoSRV.Get(),
				mesh->normalSRV.Get(),
				mesh->aoSRV.Get(),
				mesh->metallicRoughnessSRV.Get(),
				mesh->emissiveSRV.Get()
			};

			context->PSSetShaderResources(0, UINT(resViews.size()), resViews.data());
			context->VSSetShaderResources(0, 1, mesh->heightSRV.GetAddressOf());

			context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);
			context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			context->DrawIndexedInstanced(mesh->indexCount, static_cast<UINT>(instanceCount), 0, 0, 0);
		}

	}
}

/// <summary>
/// Skinned 인스턴스들을 드로우콜을 실행하는 함수
/// </summary>
/// <param name="queue"></param>
void URenderer::RenderSkinnedInstanced(const URenderQueue& queue)
{
	const unordered_map<shared_ptr<const GPUMeshAsset>, SkinnedInstanceBatch>& batches = queue.GetSkinnedInstanceBatches();

	// 물체들 순회 [메시 그룹]
	for (unordered_map < shared_ptr<const GPUMeshAsset>, SkinnedInstanceBatch>::const_iterator it = batches.begin(); it != batches.end(); ++it)
	{
		const shared_ptr<const GPUMeshAsset>& meshAsset = it->first;
		const SkinnedInstanceBatch& batch = it->second;

		if (!meshAsset || batch.instances.empty())
			continue;

		const size_t instanceCount = batch.instances.size();
		const uint32_t maxBoneCount = batch.maxBoneCount;

		skinnedBatchConstsCPU.maxBoneCount = maxBoneCount;
		D3D11Utils::UpdateBuffer(device, context, skinnedBatchConstsCPU, skinnedBatchConstsGPU);

		// 1. 인스턴스 데이터 업로드
		EnsureSkinnedInstanceCapacity(device.Get(), instanceCount);
		for (size_t i = 0; i < instanceCount; i++)
		{
			skinnedInstances.cpuData[i].meshConstsCPU = batch.instances[i].meshConstsCPU;
			skinnedInstances.cpuData[i].materialConstsCPU = batch.instances[i].materialConstsCPU;
		}
		skinnedInstances.Upload(context.Get());

		// 2. 본 팔레트 업로드
		EnsureBonePaletteCapacity(device.Get(), instanceCount * maxBoneCount);
		for (size_t i = 0; i < instanceCount; i++)
		{
			const auto* palette = batch.instances[i].bonePaletteCPU;
			const size_t boneCount = palette->size();

			memcpy(&bonePalettes.cpuData[i * maxBoneCount],
				palette->data(),
				boneCount * sizeof(Matrix));

			for (size_t j = boneCount; j < maxBoneCount; j++)
				bonePalettes.cpuData[i * maxBoneCount + j] = Matrix();
		}
		bonePalettes.Upload(context.Get());

		// 3. 셰이더 바인딩
		ID3D11ShaderResourceView* srvs[2] = {
			skinnedInstances.GetSRV(),
			bonePalettes.GetSRV()
		};
		context->VSSetShaderResources(8, 2, srvs);
		context->PSSetShaderResources(8, 1, srvs);

		context->VSSetConstantBuffers(0, 1 , skinnedBatchConstsGPU.GetAddressOf());

		// 4. DrawIndexedInstanced
		for (const auto& mesh : meshAsset->meshes)
		{
			std::vector<ID3D11ShaderResourceView*> resViews =
			{
				mesh->albedoSRV.Get(),
				mesh->normalSRV.Get(),
				mesh->aoSRV.Get(),
				mesh->metallicRoughnessSRV.Get(),
				mesh->emissiveSRV.Get()
			};

			context->PSSetShaderResources(0, (UINT)resViews.size(), resViews.data());
			context->VSSetShaderResources(0, 1, mesh->heightSRV.GetAddressOf());

			context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride, &mesh->offset);
			context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			context->DrawIndexedInstanced(mesh->indexCount, (UINT)instanceCount, 0, 0, 0);
		}
	}

}

void URenderer::OnResize()
{
	numQualityLevels = D3D::Get()->numQualityLevels;
	useMSAA = D3D::Get()->useMSAA;

	// width, height 변경된거 재설정하고
	// QualityLevels가 1이상이라면 Count는 4로 고정사용중
	viewport.TopLeftX = 0; viewport.TopLeftY = 0;
	viewport.Width = float(D3D::Get()->GetDesc().Width);
	viewport.Height = float(D3D::Get()->GetDesc().Height);
	viewport.MinDepth = 0.0f; viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	// Harzard 예방
	D3D11Utils::UnbindPSRange(context.Get(), 0, 32);
	context->OMSetRenderTargets(0, nullptr, nullptr);

	floatSRV.Reset();   floatRTV.Reset();   floatBuffer.Reset();
	resolvedSRV.Reset(); resolvedRTV.Reset(); resolvedBuffer.Reset();
	postEffectsSRV.Reset(); postEffectsRTV.Reset(); postEffectsBuffer.Reset();
	

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
	ThrowIfFailed(device->CreateTexture2D(&desc, nullptr, floatBuffer.GetAddressOf()));
	ThrowIfFailed(device->CreateRenderTargetView(floatBuffer.Get(), nullptr, floatRTV.GetAddressOf()));
	ThrowIfFailed(device->CreateShaderResourceView(floatBuffer.Get(), nullptr, floatSRV.GetAddressOf()));

	// 2. 리졸브용
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	ThrowIfFailed(device->CreateTexture2D(&desc, nullptr, resolvedBuffer.GetAddressOf()));
	ThrowIfFailed(device->CreateRenderTargetView(resolvedBuffer.Get(), nullptr, resolvedRTV.GetAddressOf()));
	ThrowIfFailed(device->CreateShaderResourceView(resolvedBuffer.Get(), nullptr, resolvedSRV.GetAddressOf()));

	// 3. posteffects용
	ThrowIfFailed(device->CreateTexture2D(&desc, nullptr, postEffectsBuffer.GetAddressOf()));
	ThrowIfFailed(device->CreateRenderTargetView(postEffectsBuffer.Get(), nullptr, postEffectsRTV.GetAddressOf()));
	ThrowIfFailed(device->CreateShaderResourceView(postEffectsBuffer.Get(), nullptr, postEffectsSRV.GetAddressOf()));

	// 포스트프로세스 재설정
	postProcess.OnResize
	(device, context,
		{ postEffectsSRV }, { D3D::Get()->GetFinalLDR_RTV() },
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
	shadowViewport.Width = float(shadowWidth);
	shadowViewport.Height = float(shadowHeight);
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
		const LightData& light = globalConstsCPU.lights[i];

		if ((light.type & LIGHT_SHADOW) == false)
			continue;

		Vector3 up(0.f, 1.f, 0.f);

		float dot = up.Dot(light.direction);

		if (fabs(dot) > 0.99f)
			up = Vector3(1.0f, 0.0f, 0.0f);

		Matrix lightViewRow = XMMatrixLookAtLH(light.position, light.position + light.direction, up);
		Matrix lightProjRow = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), 1.0f, 0.01f, 100.0f);

		shadowGlobalConstsCPU[i].eyeWorld = light.position;
		shadowGlobalConstsCPU[i].view = lightViewRow.Transpose();
		shadowGlobalConstsCPU[i].proj = lightProjRow.Transpose();
		shadowGlobalConstsCPU[i].invProj = lightProjRow.Invert().Transpose();
		shadowGlobalConstsCPU[i].viewProj = (lightViewRow * lightProjRow).Transpose();

		globalConstsCPU.lights[i].viewProj = shadowGlobalConstsCPU[i].viewProj;
		globalConstsCPU.lights[i].invProj = shadowGlobalConstsCPU[i].invProj;

		D3D11Utils::UpdateBuffer(device, context, shadowGlobalConstsCPU[i], shadowGlobalConstsGPU[i]);
	}
	D3D11Utils::UpdateBuffer(device, context, globalConstsCPU, globalConstsGPU);
}


void URenderer::RenderPostProcess()
{
	const float clearColor[4] = { 0, 0, 0, 1 };

	// Clear 백버퍼 (안 하면 겹침 생길 수 있음)
	context->ClearRenderTargetView(D3D::Get()->GetBackBufferRTV().Get(), clearColor);

	// Harzard방지, Resolve전 SRV가 바인딩 되어있다면 해제
	D3D11Utils::UnbindIfBoundPS(context.Get(), resolvedSRV.Get());

	context->ResolveSubresource
	(
		resolvedBuffer.Get(),				 // 대상 (싱글샘플 텍스처)
		0,									 // 대상 서브리소스 index
		floatBuffer.Get(),				 // 소스 (MSAA 텍스처)
		0,									 // 소스 서브리소스 index
		DXGI_FORMAT_R16G16B16A16_FLOAT		 // 포맷 일치해야 함
	);

	// PostEffects
	Graphics::postEffectsPSO.Apply(context.Get());
	D3D11Utils::UpdateBuffer(device,context, postEffectsConstsCPU, postEffectsConstsGPU);

	// ----------------그림자 맵 확인------------------------
	/*SetGlobalConsts(shadowGlobalConstsGPU[1].Get());
	vector<ID3D11ShaderResourceView*> postEffectsSRVs = 
	{
		resolvedSRV.Get(), shadowSRVs[1].Get() 
	};
	context->PSSetShaderResources(20, UINT(postEffectsSRVs.size()),postEffectsSRVs.data());
	context->OMSetRenderTargets(1, postEffectsRTV.GetAddressOf(), NULL);
	context->PSSetConstantBuffers(3, 1,postEffectsConstsGPU.GetAddressOf());
	postEffects.Render(context);*/

	// -------------------------------------------------------
		
	D3D11Utils::UnbindPSRange(context.Get(), 20, 8);
	D3D11Utils::UnbindIfBoundPS(context.Get(), postEffectsSRV.Get());

	vector<ID3D11ShaderResourceView*> postEffectsSRVs =
	{
		resolvedSRV.Get(), D3D::Get()->GetDepthOnly_SRV().Get()
	};

	context->PSSetShaderResources(20, UINT(postEffectsSRVs.size()), postEffectsSRVs.data());

	context->OMSetRenderTargets(1, postEffectsRTV.GetAddressOf(), NULL);
	context->PSSetConstantBuffers(3, 1, postEffectsConstsGPU.GetAddressOf());
	postEffects.Render(context);

	// -------------------------------------------------------

	// PostProcess
	Graphics::postProcessingPSO.Apply(context.Get());

	postProcess.combineFilter.UpdateConstantBuffers(device, context);
	postProcess.Render(context);
}

void URenderer::Present()
{
	D3D::Get()->Present();
}

// ----------------------------------------------- private Util -----------------------------------------------

void URenderer::EnsureInstanceCapacity(ID3D11Device* device, size_t needed)
{
	if (instanceCapacity >= needed && instances.gpuData) return;		// 크기가 만약 넉넉하다면 리턴

	const size_t newCap = NextPow2(needed > 0 ? needed : 1);
	instances.cpuData.resize(newCap);									// cpu데이터 공간 확보
	instances.Initialize(device);										// cpu데이터 공간을 기반으로 SRV생성
	instanceCapacity = static_cast<UINT>(newCap);
}

void URenderer::EnsureSkinnedInstanceCapacity(ID3D11Device* device, size_t needed)
{
	if (skinnedInstanceCapacity >= needed && skinnedInstances.gpuData)
		return;

	const size_t newCap = NextPow2(needed > 0 ? needed : 1);
	skinnedInstances.cpuData.resize(newCap);
	skinnedInstances.Initialize(device);
	skinnedInstanceCapacity = static_cast<UINT>(newCap);
}

void URenderer::EnsureBonePaletteCapacity(ID3D11Device* device, size_t needed)
{
	if (bonePaletteCapacity >= needed && bonePalettes.gpuData)
		return;

	const size_t newCap = NextPow2(needed > 0 ? needed : 1);
	bonePalettes.cpuData.resize(newCap);
	bonePalettes.Initialize(device);
	bonePaletteCapacity = static_cast<UINT>(newCap);
}
