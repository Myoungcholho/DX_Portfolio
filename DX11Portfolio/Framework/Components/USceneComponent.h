#pragma once

#include <memory>

#include "UActorComponent.h"
#include "Transform.h"

class USceneComponent : public UActorComponent
{
public:
    USceneComponent();

protected:
    // shared_ptr은 굳이? Transform은 Value타입에 가깝기 때문에 소유권 공유 필요가 없음
	Transform m_relativeTransform;		                // 부모 기준 위치(상대 위치)
	Transform m_worldTransform;		                    // 렌더링에 사용(절대 위치)

	USceneComponent* m_parent;                          // 부모 포인터
	std::vector<USceneComponent*> m_children;           // 자식 포인터

public:
    void SetRelativePosition(const Vector3& position);
    void SetRelativeRotation(const Quaternion& rotation);
    void SetRelativeRotation(const Vector3& rotation);
    void SetRelativeScale(const Vector3& scale);
    void SetRelativeTransform(const Transform& t);      // 부모를 기준으로 하는 로컬 트랜스폼을 설정
    void UpdateWorldTransform();
    void UpdateWorldTransformRecursive();               

    // 월드행렬 반환
    //const Transform& GetWorldTransform() const;

    // 계층 구조 구성
    void AttachTo(USceneComponent* parent);
    void Detach();

public:
    const std::vector<USceneComponent*>& GetChildren() const;
    USceneComponent* GetParent() const;
    USceneComponent* GetRoot() const;
};