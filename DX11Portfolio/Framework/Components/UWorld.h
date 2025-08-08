#pragma once

#include <vector>

#include "AActor.h"

class URenderManager;

class UWorld : public UObject
{
public:
    //URenderQueue* GetRenderQueue() { return &m_renderQueue; }

public:
    template<typename T>
    T* SpawnActor()
    {
        static_assert(std::is_base_of<AActor, T>::value);
        T* actor = new T();

        actor->SetWorld(this);                  // 월드 주입
        //actor->init();                        // DI때문에 미룸
        //actor->BeginPlay();                   
        Actors.push_back(actor);

        return actor;
    }

    // void ObjectInit(); // Framework가 Game에 의존하면 안된다.
    void Initialize();
    void Tick();
    void Render();
    void Destroy() override;
    void OnGUI();

    void StartAllActors();
public:
    void SetRenderManager(URenderManager* manager) { m_renderManager = manager; }

private:
    vector<AActor*> Actors;

private:
    URenderManager* m_renderManager = nullptr;
};