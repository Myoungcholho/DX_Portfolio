#pragma once

class IExecutable
{
public:
	virtual void Initialize() {}
	virtual void Destroy() {}

	virtual void Tick() {}
	virtual void UpdateGUI() {}
	virtual void Render() {}
};