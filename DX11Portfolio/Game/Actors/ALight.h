#pragma once

class ALight : public AActor
{
public:
	ALight();

public:
	void Initialize() override;

public:
	ULightComponent* GetLightComponent() { return lightComponent; }

private:
	ULightComponent* lightComponent;
};