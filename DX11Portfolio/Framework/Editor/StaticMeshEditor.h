#pragma once

class UPrimitiveComponent;
struct MaterialConstants;

class PrimitiveEditor : public Editor
{
public:
	explicit PrimitiveEditor(UPrimitiveComponent* target = nullptr);

	void SetTarget(UPrimitiveComponent* target);
	void OnGUI() override;

private:
	void RefreshBuffersFromTarget();		 // Å¸°Ù °ª ¡æ UI ¹öÆÛ
	void ApplyBuffersToTarget();			 // UI ¹öÆÛ ¡æ Å¸°Ù °ª

private:
	UPrimitiveComponent* mTarget = nullptr;

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