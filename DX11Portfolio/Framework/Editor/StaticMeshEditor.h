#pragma once

class UStaticMeshComponent;
struct MaterialConstants;

class StaticMeshEditor : public Editor
{
public:
	explicit StaticMeshEditor(UStaticMeshComponent* target = nullptr);

	void SetTarget(UStaticMeshComponent* target);
	void OnGUI() override;

private:
	void RefreshBuffersFromTarget();		 // Å¸°Ù °ª ¡æ UI ¹öÆÛ
	void ApplyBuffersToTarget();			 // UI ¹öÆÛ ¡æ Å¸°Ù °ª

private:
	UStaticMeshComponent* mTarget = nullptr;

	// Data
	float metallicFactor;
	float roughnessFactor;
	int useAlbedoMap;
	int useEmissiveMap;
	int useNormalMap;
	int useAOMap;
	int useMetallicMap;
	int useRoughnessMap;

	int useHeightMap;
	float heightScale;

	bool drawNormals = false;
};