#pragma once

class ULightComponent : public USceneComponent
{
public:
    ULightComponent();

public:
    virtual void OnAttach(AActor* owner) override;
    virtual void OnDetach() override;

    void SetRadiance(const Vector3& value) { radiance = value; }
    void SetFalloff(float start, float end) { fallOffStart = start; fallOffEnd = end; }
    void SetSpotPower(float power) { spotPower = power; }
    void SetType(LightType t) { type = t; }

private:
    Vector3 radiance = Vector3(1.0f, 1.0f, 1.0f);      // 빛의 색과 세기
    float fallOffStart = 1.0f;                         // Point/Spot일때 사용

    float fallOffEnd = 10.0f;                          // Point/Spot일때 사용
    float spotPower = 16.0f;                           // Spot일때 사용

    LightType type;                                    // 0 : Directional, 1 : Point, 2 : Spot
};