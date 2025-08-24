#include "Framework.h"
#include "URenderManager.h"

URenderManager::URenderManager()
{
	context = D3D::Get()->GetDeviceContext();
}

URenderManager::~URenderManager()
{
	Stop();
}

void URenderManager::Init()
{
	m_renderer = make_unique<URenderer>();
	m_renderer->Init();

	m_shouldExit = false;												// 렌더링 종료 플래그 기본값 설정
	m_renderThread = thread(&URenderManager::RenderLoop, this);			// 새로운 스레드로 메서드 실행
}

void URenderManager::Stop()
{
	// 스레드 안전하게 종료
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_shouldExit = true;
		m_renderReady = true;
	}

	// 조건 변수를 깨워서 내부 대기 상태 해제
	m_condition.notify_one();

	// 스레드가 실행중이라면 메인 스레드가 기다려서 종료될 때 까지 대기
	if (m_renderThread.joinable())
		m_renderThread.join();
}

void URenderManager::EnqueueProxies(vector<shared_ptr<URenderProxy>> proxies, vector<LightData> lights)
{
	// 쓰기 인덱스에 렌더링 리스트 저장하고 읽기 인덱스와 교체
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_renderQueues[m_writeIndex] = std::move(proxies);
		m_renderLightData[m_writeIndex] = lights;

		std::swap(m_writeIndex, m_readIndex);
		m_renderReady = true;
	}

	// 조건 변수 깨워서 RenderLoop()가 새 프레임을 렌더링하도록 알림
	m_condition.notify_one();
}

void URenderManager::RenderLoop()
{
	// 종료플레그가 있다면 돌지 않음
	while (!m_shouldExit)
	{
		PRO_BEGIN(L"RenderFrame");
		std::vector<std::shared_ptr<URenderProxy>> proxiesToRender;

		{
			// 대기하다가 렌더링이 준비되거나 종료 플래그가 true일때 탈출
			std::unique_lock<std::mutex> lock(m_mutex);	
			m_condition.wait(lock, [&]() { return m_renderReady || m_shouldExit; });

			if (m_shouldExit)
				break;
			
			// 읽기 인덱스의 렌더 큐를 가져와 렌더링할 목록으로 복사
			proxiesToRender = std::move(m_renderQueues[m_readIndex]);
			m_renderReady = false;
		}

		// 전역 상수 설정
		Vector3 eye = CContext::Get()->GetCamera()->GetPosition();
		Matrix view = CContext::Get()->GetViewMatrix();
		Matrix proj = CContext::Get()->GetProjectionMatrix();

		// Renderer가 가지는 GlobalConsts 값 셋팅
		m_renderer->UpdateGlobalLights(m_renderLightData[m_readIndex]);
		m_renderer->UpdateGlobalConstants(eye, view, proj);
		
		// 렌더큐에 넣어서 내부에서 분류
		URenderQueue renderQueue;
		for (auto& proxy : proxiesToRender)
		{
			// 넣기전에 CPU 데이터 얻어서 복사해서 프록시로 넘겨주기


			renderQueue.AddProxy(std::move(proxy));  // 내부에서 분류
		}

		// 인자로 렌더큐를 전달해서 렌더링 시작
		m_renderer->RenderFrame(renderQueue);

		// 후처리 및 출력
		m_renderer->RenderPostProcess();

		//ImGuiManager::Get()->RenderDrawData(context);

		EditorApplication::Run();

		m_renderer->Present();
		PRO_END(L"RenderFrame");
	}
}

void URenderManager::OnGUI()
{
	// 전역 렌더링 값 수정
	if (m_renderer)
		m_renderer->OnGUI();
}