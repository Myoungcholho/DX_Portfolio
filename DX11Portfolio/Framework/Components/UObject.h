#pragma once

/// <summary>
/// UObject로부터 기대할 수 있는 것은 모든 클래스를 다운캐스팅 업캐스팅하여 사용할 수 있음이고
/// 1. 따라서 가비지컬렉터 대상으로 판별로 응용이 가능
/// 2. 리플렉션 기능 구현할 때 리펙토링
/// </summary>
class UObject
{
public:
    virtual ~UObject() = default;
    //virtual void Tick() {} , Tick은 ActorComponent이상에서 가능하도록 설계
    //virtual void Render() {} , Render는 Primitive 이상에서 가능하도록 설계할 것
};