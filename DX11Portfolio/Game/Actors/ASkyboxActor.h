#pragma once

class ASkyboxActor : public AActor
{
public:
	ASkyboxActor();
	
public:
	void Initialize() override;

public:
	USkyboxComponent* GetSkyboxComponent() { return skyboxComponent; }

private:
	USkyboxComponent* skyboxComponent;
};