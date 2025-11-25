#include "Framework.h"
#include "USceneComponent.h"

USceneComponent::USceneComponent()
{
    mName = "USceneComponent";

    localTransform = Transform();
    worldTransform = Transform();
    parent = nullptr;
}

/// <summary>
/// RelativeTransform Setting
/// </summary>
void USceneComponent::SetRelativeTransform(const Transform& t)
{
    localTransform = t;                                                    // 부모가 누군진 모르지만 기준으로 한 Transform
    UpdateWorldTransformRecursive();                                                     // WorldMatrix에 반영, 왜냐하면 렌더링할때는 이 정보가 사용되니
}

void USceneComponent::SetRelativePosition(const Vector3& position)
{
    localTransform.SetPosition(position);
    UpdateWorldTransformRecursive();
}

void USceneComponent::SetRelativeRotation(const Quaternion& rotation)
{
    localTransform.SetRotation(rotation);
    UpdateWorldTransformRecursive();
}

void USceneComponent::SetRelativeRotation(const Vector3& rotation)
{
    localTransform.SetRotation(rotation);
    UpdateWorldTransformRecursive();
}

void USceneComponent::SetRelativeScale(const Vector3& scale)
{
    localTransform.SetScale(scale);
    UpdateWorldTransformRecursive();
}

/// <summary>
/// RelativeTransform으로 Parent유무에 따라 WorldMatrix값을 업데이트
/// </summary>
void USceneComponent::UpdateWorldTransform()
{
    if (parent)
    {
#ifdef TransformTest
        cout << "--------------------------------------------------\n";
        cout << "Parent Before :" << parent->mName << "\n";
        cout << "Pos :" << parent->worldTransform.GetPosition().x << "," << parent->worldTransform.GetPosition().y << "," << parent->worldTransform.GetPosition().z << "\n";
        cout << "Rot :" << parent->worldTransform.GetRotation().x << "," << parent->worldTransform.GetRotation().y << "," << parent->worldTransform.GetRotation().z << "\n";
        cout << "Scale :" << parent->worldTransform.GetScale().x << "," << parent->worldTransform.GetScale().y << "," << parent->worldTransform.GetScale().z << "\n";
#endif
        worldTransform = localTransform * parent->worldTransform;    // World행렬을 부모행렬과 상대행렬의 곱으로 구성

#ifdef TransformTest
        cout << "Name after :" << mName << "\n";
        cout << "Pos :" << worldTransform.GetPosition().x << "," << worldTransform.GetPosition().y << "," << worldTransform.GetPosition().z << "\n";
        cout << "Rot :" << worldTransform.GetRotation().x << "," << worldTransform.GetRotation().y << "," << worldTransform.GetRotation().z << "\n";
        cout << "Scale :" << worldTransform.GetScale().x << "," << worldTransform.GetScale().y << "," << worldTransform.GetScale().z << "\n";
#endif
    }
    else
    {
        worldTransform = localTransform;                                 // 부모가 없다면 상대위치는 곧 월드위치
    }
}


/// <summary>
/// 현재 컴포넌트의 WorldTransform을 갱신하고 모든 자식의 WorldTransform도 재귀적으로 갱신
/// </summary>
void USceneComponent::UpdateWorldTransformRecursive()
{
    UpdateWorldTransform();

    for (USceneComponent* child : children)
    {
        if (child)
            child->UpdateWorldTransformRecursive();
    }
}

Vector3 USceneComponent::GetActorWorldPosition()
{
    UpdateWorldTransform();

    return worldTransform.GetPosition();
}

Transform USceneComponent::GetRelativeTransform() const
{
    return localTransform;
}

Transform USceneComponent::GetWorldTransform() const
{
    return worldTransform;
}

bool USceneComponent::AttachTo(USceneComponent* newParent, EAttachMode mode)
{
    if (parent == newParent) return true;
    if (newParent == this) return false;

    // 새 부모의 조상들을 위로 타고 올라가면서 검사하며
    // 내가 이미 위쪽에 있다면 Cycle이 생기므로 금지(false)
    for (auto* p = newParent; p != nullptr; p = p->GetParent())
        if (p == this) return false;

    UpdateWorldTransform();
    Matrix curWorld = worldTransform.GetWorldMatrix();

    // 기존 부모로부터 분리
    if (parent)
    {
        auto& s = parent->children;
        s.erase(remove(s.begin(), s.end(), this), s.end());
    }

    // 새 부모로 설정
    parent = newParent;
    if (parent)
    {
        parent->children.push_back(this);
        //UpdateWorldTransform();
    }

    // 트랜스폼 모드 적용
    switch (mode)
    {
    case EAttachMode::KeepWorld:        // 화면에서 안움직이게 유지하고 싶다!
    {
        const Matrix parentWorld = (parent) ? parent->GetWorldMatrix() : Matrix();
        const Matrix newLocal = parentWorld.Invert() * curWorld;
        localTransform.SetWorldMatrix(newLocal);
        break;
    }
    case EAttachMode::KeepRelative:     // local값은 유지하고 싶다!
    {
        // relative는 그대로 World는 부모 기준으로 업데이트
        break;
    }
    case EAttachMode::SnapToTarget:     // 부모의 위치에 따라가고 싶다!
    {
        // relativeTransform 초기화
        localTransform = Transform{};
        break;
    }
    }

    UpdateWorldTransformRecursive();
    return true;
}

bool USceneComponent::Detach(EAttachMode mode)
{
    UpdateWorldTransform();
    const Matrix curWorld = worldTransform.GetWorldMatrix();

    if (parent)
    {
        vector<USceneComponent*>& s = parent->children;
        s.erase(remove(s.begin(), s.end(), this), s.end());
        parent = nullptr;
    }
    else
    {
        return true;
    }

    switch (mode)
    {
    case EAttachMode::KeepWorld:
        // 부모 없음이면 World=Local → 현재 월드를 로컬에 굳힘
        localTransform.SetWorldMatrix(curWorld);
        break;

    case EAttachMode::KeepRelative:
        // 상대 유지 → 부모 사라졌으니 결과적으로 월드가 바뀜 (아무 것도 안 함)
        break;

    case EAttachMode::SnapToTarget:
        // 월드 원점으로 스냅하고 싶으면 Identity, 아니면 정책에 맞춰 조정
        localTransform = Transform{};
        break;
    }

    UpdateWorldTransformRecursive();
    return true;
}

Vector3 USceneComponent::GetRelativePosition() const
{
    return localTransform.GetPosition();
}

Vector3 USceneComponent::GetRelativeRotationEuler() const
{
    return localTransform.GetRotation();
}

Quaternion USceneComponent::GetRelativeRotationQuat() const
{
    return localTransform.GetRotationQuat();
}

Vector3 USceneComponent::GetRelativeScale() const
{
    return localTransform.GetScale();
}

const vector<USceneComponent*>& USceneComponent::GetChildren() const
{
    return children;
}


USceneComponent* USceneComponent::GetParent() const
{
    return parent;
}

Vector3 USceneComponent::GetForwardVector() const
{
    return localTransform.GetForward();
}

USceneComponent* USceneComponent::GetRoot() const
{
    if (parent == nullptr)
        return const_cast<USceneComponent*>(this);
    return parent->GetRoot();
}

const Matrix& USceneComponent::GetWorldMatrix()
{
    UpdateWorldTransform();
    return worldTransform.GetWorldMatrix();
}