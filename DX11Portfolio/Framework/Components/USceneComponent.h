#pragma once

#include <memory>

#include "UActorComponent.h"
#include "Transform.h"

// KeppWorld : 보이는 월드 위치/회전/스케일을 그대로 유지하며 자식/부모 업데이트
// KeepRelative : 로컬을 유지하며 부모가 바뀌므로 월드 위치/회전/스케일이 변경될 수 있음
// SnapToTarget : 자식 로컬을 Identity로 변경해 부모의 위치를 그대로 사용
enum class EAttachMode { KeepWorld, KeepRelative, SnapToTarget };

class USceneComponent : public UActorComponent
{
public:
    USceneComponent();
    ~USceneComponent() override = default;

    // 언리얼의 값 관리 형식 방식을 적용
    // Transform은 Value타입에 가깝기 때문에 소유권 공유 필요가 없음
protected:
	Transform localTransform;		                    // 부모 기준 위치(상대 위치, Local)
	Transform worldTransform;		                    // 렌더링에 사용(절대 위치, World)

	USceneComponent* parent;                            // 부모 포인터
	vector<USceneComponent*> children;                  // 자식 포인터

public:
    void SetRelativePosition(const Vector3& position);
    void SetRelativeRotation(const Quaternion& rotation);
    void SetRelativeRotation(const Vector3& rotation);
    void SetRelativeScale(const Vector3& scale);
    void SetRelativeTransform(const Transform& t);      // 부모를 기준으로 하는 로컬 트랜스폼을 설정
    Vector3 GetRelativePosition() const;
    Vector3 GetRelativeRotationEuler() const;
    Quaternion GetRelativeRotationQuat() const;
    Vector3 GetRelativeScale() const;
    Vector3 GetForwardVector() const;
    void UpdateWorldTransform();
    void UpdateWorldTransformRecursive();
    Vector3 GetActorWorldPosition();

    // 로컬Transform
    Transform GetRelativeTransform() const;
    Transform GetWorldTransform() const;

    // 월드행렬 반환
    const Matrix& GetWorldMatrix();

    // 계층 구조 구성
    bool AttachTo(USceneComponent* parent, EAttachMode mode);
    bool Detach(EAttachMode mode);

public:
    USceneComponent* GetParent() const;
    const vector<USceneComponent*>& GetChildren() const;
    USceneComponent* GetRoot() const;
};