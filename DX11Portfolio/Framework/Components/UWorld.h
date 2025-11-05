#pragma once

#include <vector>

class AActor;
class URenderManager;

class UWorld : public UObject
{
public:
    template<typename T, typename... Args>
    requires derived_from<T,AActor> && constructible_from<T, Args...>           // 컴파일 타임 제약, T는 반드시 AActor를 상속하여야함, T생성자가 반드시 존재해야함
    T* SpawnActor(Args&&... args)                                               // 전달참조 r-value만 받는다가 아님
    {
        // Spawn팩토리는 호출자가 넘긴 값 범주를 그대로 보존해야한다.
        // 생성자가 &를 받는다면 l-value값을 받아야한다, 복사/참조 용이고
        // 생성자가 &&를 받는다면 이동 생성자 호출로 복사 비용을 절약용일수도 있기 때문
        auto up = make_unique<T>(forward<Args>(args)...);
        T* raw = up.get();

        raw->SetWorld(this);                                // World Injection        
        Actors.push_back(move(up));

        return raw;
    }

    // void ObjectInit(); // Framework가 Game에 의존하면 안된다.
    void Initialize();
    void Tick();
    void FixedTick(double fixedDt);                                             // Timer클래스의 Delta값과 고정Tick에서의 Delta값은 1:1로 동기가 아니므로 인자로 delta를 계산해 받게함.
    void ProxySnapshot();
    void Destroy();
    void OnGUI();

    void StartAllActors();

public:
    vector<AActor*> GetActorsOf() const;

public:
    void SetRenderManager(URenderManager* manager) { m_renderManager = manager; }

public:
    void MarkActorForDestroy(AActor* actor);
    void FlushDestroyed();

private:
    void RemoveActor(AActor* actor);

private:
    vector<unique_ptr<AActor>> Actors;
    vector<AActor*> PendingDestroy;

private:
    URenderManager* m_renderManager = nullptr;
};