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
	URenderer* GetRenderer() const { return renderer.get(); }

public:
	void Init();
	void Stop();

	void EnqueueProxies(vector<shared_ptr<URenderProxy>>&& proxies, vector<LightData>&& lights);

private:
	void RenderLoop();

private:
	static constexpr int BUFFER_COUNT = 2;
	thread renderThread;
	atomic<bool> shouldExit = false;

	mutex mtx;
	condition_variable condition;

	vector<shared_ptr<URenderProxy>> renderQueues[2];
	vector<LightData> renderLightData[2];

	int writeIndex = 0;
	int readIndex = 1;
	bool renderReady = false;

private:
	unique_ptr<URenderer> renderer;

};