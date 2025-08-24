#pragma once

class InspectorWindow : public EditorWindow
{
public:
	InspectorWindow();
	~InspectorWindow();

	void Initialize() override;
	void Run() override;
	void Update() override;
	void OnGUI() override;
	void OnEnable() override;
	void OnDisable() override;
	void OnDestroy() override;

public:
	void OnUpdateTarget();
	FDelegateHandle m_InspectorHandle;
	FDelegateHandle m_InspectorComponentHandle;

private:
	std::vector<unique_ptr<Editor>> mEditors;
	
	//FDelegateHandle m_seletedHandle;
	//FDelegateHandle m_lightHandle;
};