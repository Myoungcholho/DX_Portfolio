#pragma once

#include "Systems/IExecutable.h"

class ExecutionManager : public IExecutable
{
public:
	virtual void Initialize();
	virtual void Destroy();
	virtual void UpdateGUI();
	virtual void Tick();
	virtual void Render();

private:
	void CreateExecutables();

private:
	void LightCreate();

private:
	vector<IExecutable*> executes;
};