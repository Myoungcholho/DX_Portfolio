#pragma once

class ASkyboxActor : public AActor
{
public:
	ASkyboxActor();
	
public:
	void Initialize() override;

public:
	shared_ptr<USkyboxComponent> GetSkyboxComponent();

private:
	shared_ptr<USkyboxComponent> m_skyboxComponent;
};