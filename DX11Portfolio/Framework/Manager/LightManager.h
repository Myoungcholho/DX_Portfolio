#pragma once

class LightManager
{
public:
    LightManager();
    ~LightManager();

public:
    void AddLight(const LightData& light);      // 라이트 등록
    void RemoveLight(int id);               // 라이트 제거
    void ClearLights();                     // 모든 라이트 제거

    void UpdateCBuffer(LightConstantBuffer& outData, const Vector3& receiverPosition ,size_t maxCount = MAX_LIGHTS);

private:
    std::vector<LightData> lights;
};