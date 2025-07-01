#include "Pch.h"
#include "ExecutionManager.h"

void ExecutionManager::CreateExecutables()
{
	executes.push_back(new BillboardSystem());
	executes.push_back(new CubeMapSkyRenderer());
	//
	//// 물체
	executes.push_back(new TestObjectCube());
	executes.push_back(new TestObjectQuad());
	executes.push_back(new TestObjectPlane());
	executes.push_back(new TestObjectGrid());
	executes.push_back(new TestObjectCylinder());
	executes.push_back(new TestObjectSphere());
	executes.push_back(new TestObjectModel());
}

void ExecutionManager::CreateFilters()
{
	filters.clear();

	float m_down = 32;

	// 1. Copy만 [고해상도 렌더링 사본]
	auto copyFilter = make_shared<ImageFilter>(L"Sampling", L"Sampling", 
		D3D::Get()->GetDesc().Width, D3D::Get()->GetDesc().Height);
	copyFilter->SetShaderResources({ D3D::Get()->GetMainShaderResourceView() });
	filters.push_back(copyFilter);

	// 2. Down Sampling Blur 처리
	auto downFilter = make_shared<ImageFilter>(L"Sampling", L"Sampling", 
		D3D::Get()->GetDesc().Width / m_down, D3D::Get()->GetDesc().Height / m_down);
	downFilter->SetShaderResources({ D3D::Get()->GetMainShaderResourceView() });
	downFilter->m_pixelConstData.threshold = CContext::Get()->GetThreshold();
	downFilter->Tick();
	filters.push_back(downFilter);

	for (int i = 0; i < 10; ++i)
	{
		auto& prevResource = filters.back()->m_shaderResourceView;
		filters.push_back(make_shared<ImageFilter>(L"Sampling", L"BlurX",
			D3D::Get()->GetDesc().Width / m_down, D3D::Get()->GetDesc().Height / m_down));
		
		filters.back()->SetShaderResources({prevResource});

		auto &prevResource2 = filters.back()->m_shaderResourceView;
		filters.push_back(make_shared<ImageFilter>(L"Sampling", L"BlurY",
			D3D::Get()->GetDesc().Width / m_down, D3D::Get()->GetDesc().Height / m_down));

		filters.back()->SetShaderResources({prevResource2});
	}

	auto combineFilter = make_shared<ImageFilter>(L"Sampling", L"Combine",
		D3D::Get()->GetDesc().Width, D3D::Get()->GetDesc().Height);

	combineFilter->SetShaderResources({ copyFilter->m_shaderResourceView,  filters.back()->m_shaderResourceView});
	// Main RTV로 [SwapChain에 등록된 텍스처]
	combineFilter->SetRenderTargets({ D3D::Get()->GetMainRenderTargetView() });
	combineFilter->m_pixelConstData.strength = CContext::Get()->GetStrength();
	combineFilter->Tick();
	filters.push_back(combineFilter);

	//auto finalFilter = make_shared<ImageFilter>(L"Sampling", L"Sampling",
	//	D3D::Get()->GetDesc().Width, D3D::Get()->GetDesc().Height);
	//auto imageFilter = std::dynamic_pointer_cast<ImageFilter>(filters.back());
	//finalFilter->SetShaderResources({ imageFilter->m_shaderResourceView });
	//finalFilter->SetRenderTargets({ D3D::Get()->GetMainRenderTargetView() });

	//finalFilter->m_pixelConstData.threshold = 0.0f;	// 마지막 필터는 평균으로 처리 X
	//finalFilter->Tick();

	//filters.push_back(finalFilter);
}

void ExecutionManager::Initialize()
{
	CreateExecutables();
	CreateFilters();

	for (IExecutable* execute : executes)
		execute->Initialize();
	/*for (const auto& filter : filters)
		filter->Initialize();*/
}

void ExecutionManager::Destroy()
{
	for (IExecutable* execute : executes)
	{
		execute->Destroy();

		if (execute != nullptr)
		{
			delete execute;
			execute = nullptr;
		}
	}

	for (auto filter : filters)
	{
		if (filter)
			filter->Destroy();
	}
	filters.clear();
}

void ExecutionManager::UpdateGUI()
{
	for (IExecutable* execute : executes)
		execute->UpdateGUI();
	for (const auto& filter : filters)
		filter->UpdateGUI();
}

void ExecutionManager::Tick()
{
	for (IExecutable* execute : executes)
		execute->Tick();

	// 사실 전부 돌 필욘 없다.
	for (const auto& filter : filters)
		filter->Tick();

	auto imageFilter = std::dynamic_pointer_cast<ImageFilter>(filters.front());
	imageFilter->m_pixelConstData.threshold = CContext::Get()->GetThreshold();
	imageFilter->Tick();

	filters[1]->m_pixelConstData.threshold = CContext::Get()->GetThreshold();
	filters[1]->Tick();
	filters.back()->m_pixelConstData.strength = CContext::Get()->GetStrength();
	filters.back()->Tick();
}

void ExecutionManager::Render()
{
	for (IExecutable* execute : executes)
		execute->Render();
	for (const auto& filter : filters)
		filter->Render();
}

void ExecutionManager::ResizeCall()
{
	// 필터크기 변경됨에 RTV, SRV 생성 다시해야함
	CreateFilters();
}