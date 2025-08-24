#pragma once

class URenderer;

class RendererSettingsWindow : public EditorWindow
{
public:
	RendererSettingsWindow();
	~RendererSettingsWindow();

	void Initialize() override;
	void Update() override;
	void OnGUI() override;
	void Run() override;

public:
	void SetRenderer(URenderer* renderer) { m_renderer = renderer; }


private:
	URenderer* m_renderer = nullptr;
};