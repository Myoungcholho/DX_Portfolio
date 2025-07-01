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

public:
	void ResizeCall();

private:
	void CreateExecutables();
	void CreateFilters();

private:
	vector<IExecutable*> executes;
	vector<shared_ptr<ImageFilter>> filters;
};