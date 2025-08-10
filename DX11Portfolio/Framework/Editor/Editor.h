#pragma once

class Editor : public Entity
{
public:
	enum class eState
	{
		Paused,
		Active,
		Disabled,
		Destroyed
	};

	Editor();
	virtual ~Editor();

	virtual void Initialize();
	virtual void Update();
	virtual void OnGUI();
	virtual void OnEnable();
	virtual void OnDisable();
	virtual void OnDestroy();

private:
	eState mState;
};