#include "Framework.h"
#include "URenderManager.h"
#include "Renders/URenderer.h"

URenderManager::URenderManager() { }
URenderManager::~URenderManager() 
{
	Stop();
}

void URenderManager::Init()
{
	renderer = make_unique<URenderer>();
	renderer->Init();

	shouldExit.store(false,memory_order_relaxed);						// 렌더링 종료 플래그 기본값 설정
	renderThread = thread(&URenderManager::RenderLoop, this);			// 새로운 스레드로 메서드 실행
}

void URenderManager::Stop()
{
	// 스레드 안전하게 종료
	{
		lock_guard<mutex> lock(mtx);
		shouldExit.store(true, memory_order_release);
		renderReady = true;
	}

	// 조건 변수를 깨워서 내부 대기 상태 해제
	condition.notify_one();

	// 스레드가 실행중이라면 메인 스레드가 기다려서 종료될 때 까지 대기
	if (renderThread.joinable())
		renderThread.join();
}

void URenderManager::EnqueueProxies(vector<shared_ptr<URenderProxy>>&& proxies, vector<LightData>&& lights)
{
	// 쓰기 인덱스에 렌더링 리스트 저장하고 읽기 인덱스와 교체
	{
		lock_guard<mutex> lock(mtx);
		renderQueues[writeIndex] = move(proxies);
		renderLightData[writeIndex] = move(lights);

		swap(writeIndex, readIndex);
		renderReady = true;
	}

	// 조건 변수 깨워서 RenderLoop()가 새 프레임을 렌더링하도록 알림
	condition.notify_one();
}

void URenderManager::RenderLoop()
{
	// 종료플레그가 있다면 돌지 않음
	while(!shouldExit.load(memory_order_acquire))
	{
		//PRO_BEGIN(L"RenderThread");

		vector<shared_ptr<URenderProxy>> proxiesToRender;
		vector<LightData> lightsToRender;
		
		// 대기하다가 렌더링이 준비되거나 종료 플래그가 true일때 탈출
		{
			unique_lock<mutex> lock(mtx);	
			condition.wait(lock, [&]() { return renderReady || shouldExit.load(memory_order_relaxed); });

			if (shouldExit.load(memory_order_acquire))
				break;
			
			// 읽기 인덱스의 렌더 큐를 가져와 렌더링할 목록으로 복사
			proxiesToRender = move(renderQueues[readIndex]);
			lightsToRender = move(renderLightData[readIndex]);

			renderReady = false;
		}

		// 전역 상수 설정[리펙토링) 카메라 컨트롤러 제작 후 코드 변경]
		Vector3 eye = CContext::Get()->GetCamera()->GetPosition();
		Matrix view = CContext::Get()->GetViewMatrix();
		Matrix proj = CContext::Get()->GetProjectionMatrix();

		// Renderer가 가지는 GlobalConsts 값 셋팅
		renderer->UpdateGlobalLights(lightsToRender);
		renderer->UpdateGlobalConstants(eye, view, proj);
		
		// 렌더큐에 넣어서 내부에서 분류
		URenderQueue renderQueue;
		for (auto& proxy : proxiesToRender)
		{
			renderQueue.AddProxy(move(proxy));
		}

		// --- 실제 드로우 제출 구간 ---
		PerfMon::BeginCpu(CpuZone::RenderThread);
		PerfMon::BeginGpu(D3D::Get()->GetDeviceContext());
		
		// 인자로 렌더큐를 전달해서 렌더링 시작
		renderer->RenderFrame(renderQueue);

		// 후처리 및 출력
		renderer->RenderPostProcess();

		EditorApplication::Run();


		PerfMon::EndGpu(D3D::Get()->GetDeviceContext());
		PerfMon::EndCpu(CpuZone::RenderThread);

		renderer->Present();
		// --- 측정 완료 ---

		PerfMon::TryResolveGpu(D3D::Get()->GetDeviceContext());

		//PRO_END(L"RenderThread");
	}
}

void URenderManager::OnGUI()
{
	// 전역 렌더링 값 수정
	if (renderer)
		renderer->OnGUI();
}