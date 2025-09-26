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
    UWorld* GetWorld(){ return world.get(); }
    URenderer* GetRenderer() { return renderManager->GetRenderer(); }

public:
    void Init();
    void Tick();
    void Render();
    void Shotdown();

    void OnGUI();

private:
    double fixedDt = 1.0 / 60.0;
    double fixedAcc = 0.0;

private:
    unique_ptr<UWorld> world;
    unique_ptr<URenderManager> renderManager;
};