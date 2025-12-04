#pragma once

class APawnTest : public APawn
{
public:
	APawnTest();

public:
	void Initialize() override;
	void Tick() override;

public:
	USkeletalMeshComponent* GetSkeletalMeshComponent() { return mesh; }

private:
	// Material Factor
	Vector3 albedo = Vector3(1.0f);
	float roughnessFactor = 0.8f;
	float metaliicFactor = 0.0f;

	// transform
	Vector3 InitPos = Vector3(0, -0.5f, -9.f);

private:
	USkeletalMeshComponent* mesh = nullptr;
	ULightComponent* attachTestlight = nullptr;
};