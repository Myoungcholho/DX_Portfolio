#include "Framework.h"
#include "USceneComponent.h"

USceneComponent::USceneComponent()
{
    m_relativeTransform = Transform();
    m_worldTransform = Transform();
    m_parent = nullptr;
}

/// <summary>
/// RelativeTransform Setting
/// </summary>
void USceneComponent::SetRelativeTransform(const Transform& t)
{
    m_relativeTransform = t;                                                    // 부모가 누군진 모르지만 기준으로 한 Transform
    UpdateWorldTransform();                                                     // WorldMatrix에 반영, 왜냐하면 렌더링할때는 이 정보가 사용되니
}

void USceneComponent::SetRelativePosition(const Vector3& position)
{
    m_relativeTransform.SetPosition(position);
    UpdateWorldTransform();
}

void USceneComponent::SetRelativeRotation(const Quaternion& rotation)
{
    m_relativeTransform.SetRotation(rotation);
    UpdateWorldTransform();
}

void USceneComponent::SetRelativeRotation(const Vector3& rotation)
{
    m_relativeTransform.SetRotation(rotation);
    UpdateWorldTransform();
}

void USceneComponent::SetRelativeScale(const Vector3& scale)
{
    m_relativeTransform.SetScale(scale);
    UpdateWorldTransform();
}

/// <summary>
/// RelativeTransform으로 Parent유무에 따라 WorldMatrix값을 업데이트
/// </summary>
void USceneComponent::UpdateWorldTransform()
{
    if (m_parent)
    {
        m_worldTransform = m_parent->m_worldTransform * m_relativeTransform;    // World행렬을 부모행렬과 상대행렬의 곱으로 구성
    }
    else
    {
        m_worldTransform = m_relativeTransform;                                 // 부모가 없다면 상대위치는 곧 월드위치
    }
}


/// <summary>
/// 현재 컴포넌트의 WorldTransform을 갱신하고 모든 자식의 WorldTransform도 재귀적으로 갱신
/// 1. d
/// </summary>
void USceneComponent::UpdateWorldTransformRecursive()
{
    UpdateWorldTransform();

    for (USceneComponent* child : m_children)
    {
        if (child)
            child->UpdateWorldTransformRecursive();
    }
}


const Transform& USceneComponent::GetWorldTransform() const
{
    return m_worldTransform;
}

void USceneComponent::AttachTo(USceneComponent* parent)
{
    if (m_parent)
    {
        // 기존 부모로부터 분리
        auto& siblings = m_parent->m_children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    m_parent = parent;

    if (m_parent)
    {
        m_parent->m_children.push_back(this);
        UpdateWorldTransform();
    }
}

void USceneComponent::Detach()
{
    if (m_parent)
    {
        auto& siblings = m_parent->m_children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
        m_parent = nullptr;

        // 부모 잃었으니 현재 상대 트랜스폼을 월드 트랜스폼으로 전환
        m_relativeTransform = m_worldTransform;
    }
}

const std::vector<USceneComponent*>& USceneComponent::GetChildren() const
{
    return m_children;
}

USceneComponent* USceneComponent::GetParent() const
{
    return m_parent;
}

USceneComponent* USceneComponent::GetRoot() const
{
    if (m_parent == nullptr)
        return const_cast<USceneComponent*>(this);
    return m_parent->GetRoot();
}
