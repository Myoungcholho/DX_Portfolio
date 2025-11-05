#pragma once

class ACharacter : public APawn
{
public:
	ACharacter();

public:
	virtual void Tick() override;
	virtual void AddMovementInput(const Vector3& dir, float sclae = 1.0f) override;

protected:
	void ApplyMovement(double dt);
	void ApplyGravity(double dt);
	void UpdateRotationByController(double dt);
	
protected:
	Vector3 m_pendingInput = Vector3(0, 0, 0);
	float moveSpeed = 6.0f;			// 초당 이동 속도 (m/s)

	// 물리엔진 적용 이후
	Vector3 m_velocity = Vector3(0, 0, 0);
	float m_jumpPower = 5.0f;       // 점프 초기 속도
	float m_gravity = 9.8f;         // 중력 가속도
	bool  m_isGrounded = true;      // 지면 여부
};