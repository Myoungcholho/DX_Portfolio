#include "Framework.h"
#include "Engine.h"

Engine* Engine::instance = nullptr;

void Engine::Create()
{
	assert(instance == nullptr);

	instance = new Engine();
}

void Engine::Destory()
{
	assert(instance != nullptr);

	delete instance;
	instance = nullptr;
}

Engine* Engine::Get()
{
	return instance;
}

void Engine::Shutdown()
{
	if (textureManager)
	{
		textureManager->ReleaseAll();
		textureManager.reset();
	}
}

TextureManager* Engine::GetTextureManager() const
{
	return textureManager.get();
}

LightManager* Engine::GetLightManager() const
{
	return lightManager.get();
}

Engine::Engine()
{
	textureManager = std::make_unique<TextureManager>();
	lightManager = std::make_unique<LightManager>();
}

Engine::~Engine()
{
	Shutdown();
}
