#pragma once

/// <summary>
/// GameInstance : 게임 전역 상태와 시스템 소유자 (씬, 렌더러 등 전체 컨텍스트)
/// World        : Actor들의 집합. 각 Actor는 컴포넌트를 통해 렌더링 데이터 제공.
/// RenderManager: 렌더링 제어 흐름을 담당. 언제 무엇을 몇 번 그릴지 결정.
/// Renderer     : 렌더링 기술을 실행. "어떻게 그릴지"를 담당 (PSO, 상수버퍼, 드로우 등)
/// </summary>
class UGameInstance
{
public:
    UGameInstance();

public:
    UWorld* GetWorld(){ return m_world.get(); }

public:
    void Init();
    void Tick();
    void Render();
    void Destroy();

    void OnGUI();

private:
    unique_ptr<UWorld> m_world;
    unique_ptr<URenderManager> m_renderManager;
};