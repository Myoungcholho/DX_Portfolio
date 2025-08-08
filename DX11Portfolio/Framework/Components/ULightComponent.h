#pragma once

// LightType
//enum class LightType : int
//{
//    Directional = 0,
//    Point = 1,
//    Spot = 2
//};

//struct Light info
//{
//    Vector3 radiance = Vector3(1.0f);                   // 빛의 세기
//    float fallOffStart = 0.0f;                          // 감쇠 시작 거리
//    Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);      // 방향
//    float fallOffEnd = 10.0f;                           // 감쇠 끝 거리
//    Vector3 position = Vector3(0.0f, 0.0f, -2.0f);      // 위치
//    float spotPower = 100.0f;                           // spot 라이트용
//    int type = 0;
//    int id = -1;
//    float dummy[2];
//};

class ULightComponent : public USceneComponent
{
protected:
    LightData m_light;

public:
    virtual void Tick() override;

    void SetRadiance(const Vector3& rad) { m_light.radiance = rad; }
    void SetFalloff(float start, float end) { m_light.fallOffStart = start; m_light.fallOffEnd = end; }
    void SetSpotPower(float power) { m_light.spotPower = power; }
    void SetLightType(int type) { m_light.type = type; }

public:
    const LightData& GetLightData();
    bool IsEnabled() const { return bEnabled; }

private:
    bool bEnabled = true;
};