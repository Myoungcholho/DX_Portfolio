#pragma once

class AController;

class APlayerController : public AController
{
public:
	APlayerController();

public:
	virtual void Tick() override;

protected:
	void HandleInput(double dt);         
	void UpdateViewRotation(double dt);

private:
	float mouseSensitivity = 0.15f;
	float pitch = 0.0f;					// 상하 누적
	float yaw = 0.0;					// 좌우 누적
};