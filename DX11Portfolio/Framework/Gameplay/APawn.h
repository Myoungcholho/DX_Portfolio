#pragma once

class AActor;
class AController;

class APawn : public AActor
{
public:
	APawn();

public:
	virtual void Tick() override;
	virtual void Destroy() override;

	virtual void OnPossessed(AController* newController);
	virtual void OnUnPossessed();

	virtual void AddMovementInput(const Vector3& direction, float scale = 1.0f);
	virtual void AddRotationInput(const Vector3& eulerDelta);

	AController* GetController() const { return Controller; }

public:
	void ApplyControllerRotation(const Quaternion& ctrlRot);

public:
	bool bUseControllerRotationYaw = true;
	bool bUseControllerRotationPitch = false;
	bool bUseControllerRotationRoll = false;

protected:
	Vector3 PendingMovementInput = Vector3(0, 0, 0);
	Vector3 PendingRotationInput = Vector3(0, 0, 0);

	AController* Controller = nullptr;

	float MoveSpeed = 1.0f;
	float RotationSpeed = 90.0f;
};