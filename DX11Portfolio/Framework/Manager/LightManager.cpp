#include "Framework.h"
#include "LightManager.h"

LightManager::LightManager()
{

}

LightManager::~LightManager()
{
	lights.clear();
}

void LightManager::AddLight(const LightData& light)
{
	lights.push_back(light);
}

void LightManager::RemoveLight(int id)
{
	for (auto it = lights.begin(); it != lights.end(); ++it)
	{
		if (it->id == id)
		{
			lights.erase(it);
			cout << "light Remove, ID :" + it->id << "\n";
			break;
		}
	}
}

void LightManager::ClearLights()
{
	lights.clear();
}

// 위치를 전달받으면, 근처에 있는 라이트들을 전부 반환해 줌
void LightManager::UpdateCBuffer(LightConstantBuffer& outData, const Vector3& receiverPosition ,size_t maxCount)
{
	size_t count = 0;
	size_t safeCount = min(maxCount, size_t(MAX_LIGHTS));

	for (const LightData& light : lights)
	{
		if (count >= safeCount)
			break;

		if (light.type != static_cast<int>(LightType::Directional))
		{
			float distSq = (light.position - receiverPosition).LengthSquared();
			if (distSq > light.fallOffEnd * light.fallOffEnd) // 제곱한것보다 멀다면 너무 멈
				continue;
		}

		outData.lights[count++] = light;
	}

	// 일단 보내야하니까 채워주기
	
	for (size_t i = count; i < safeCount; ++i)
	{
		outData.lights[i] = {};
	}

	// 실제 사용 개수 저장, 셰이더에서 루프 횟수 설정용
	outData.lightCount = static_cast<int>(count);
}