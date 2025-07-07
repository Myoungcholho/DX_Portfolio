#pragma once

class TextureManager;

class Engine
{
public:
	static Engine* Get();

	static void Create();
	static void Destory();

public:
	TextureManager* GetTextureManager() const;
	LightManager* GetLightManager() const;

private:
	Engine();
	~Engine();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

private:
	void Shutdown();

private:
	unique_ptr<TextureManager> textureManager;
	unique_ptr<LightManager> lightManager;

	static Engine* instance;
};