#pragma once

class URenderManager
{
public:
	URenderManager();

public:
	void Init();
	void Render(UWorld* world);
	void OnGUI();

private:
	unique_ptr<URenderer> m_renderer;
};