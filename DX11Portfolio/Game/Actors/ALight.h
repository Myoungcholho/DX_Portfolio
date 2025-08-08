#pragma once

class ALight : public AActor
{
public:
	ALight();

public:
	void Initialize() override;

public:
	shared_ptr<ULightComponent> GetLightComponent();

private:
	shared_ptr<ULightComponent> m_lightComponent;

};