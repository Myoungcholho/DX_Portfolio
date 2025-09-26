#pragma once

struct RenderFrameData
{
    vector<shared_ptr<URenderProxy>> proxies;
    vector<LightData> lights;
};

class URenderer;

class URenderManager
{
public:
	URenderManager();
	~URenderManager();

public:
	URenderer* GetRenderer() const { return m_renderer.get(); }

public:
	void Init();
	void Stop();

	void EnqueueProxies(vector<shared_ptr<URenderProxy>>&& proxies, vector<LightData>&& lights);

public:
	void OnGUI();

private:
	void RenderLoop();

private:
	thread m_renderThread;
	atomic<bool> m_shouldExit = false;

	mutex m_mutex;
	condition_variable m_condition;

	vector<shared_ptr<URenderProxy>> m_renderQueues[2];
	vector<LightData> m_renderLightData[2];

	int m_writeIndex = 0;
	int m_readIndex = 1;
	bool m_renderReady = false;

private:
	unique_ptr<URenderer> m_renderer;
};