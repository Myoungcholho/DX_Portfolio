#pragma once
#include <queue>
#include "Components/USceneComponent.h"

// Transform의 Packet
struct FTransformUpdateCommand
{
    USceneComponent* target = nullptr;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
};

class FEditorCommandQueue
{
public:
    static void Enqueue(const FTransformUpdateCommand& cmd)
    {
        lock_guard<mutex> lock(mtx);
        que.push(cmd);
    }

    static void ProcessAll()
    {
        lock_guard<mutex> lock(mtx);
        while (que.size())
        {
            const FTransformUpdateCommand& cmd = que.front();
            if (cmd.target)
            {
                cmd.target->SetRelativePosition(cmd.position);
                cmd.target->SetRelativeRotation(cmd.rotation);
                cmd.target->SetRelativeScale(cmd.scale);
                cmd.target->UpdateWorldTransformRecursive();
            }
            que.pop();
        }
    }

private:
    static inline mutex mtx;
    static inline queue<FTransformUpdateCommand> que;
    // 다른 에디터도 변경하려면 구조체 추가후 que추가해서 사용
};