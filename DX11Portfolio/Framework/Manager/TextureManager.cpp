#include "Framework.h"
#include "TextureManager.h"

void TextureManager::Initialize()
{

}

void TextureManager::Register(const std::wstring& name, ID3D11ShaderResourceView* srv)
{
	assert(srv != nullptr);
	if (!Contains(name))
	{
		textures[name] = srv;
	}
}

void TextureManager::UnRegister(const std::wstring& name)
{
	auto it = textures.find(name);
	if (it != textures.end())
		textures.erase(it);
}

// 받아 사용하는 것은 ComPtr로 래핑해 장기 보관X
// 약한 참조 유지 해야함.
ID3D11ShaderResourceView* TextureManager::Get(const std::wstring& name) const
{
	auto it = textures.find(name);
	if (it != textures.end())
		return it->second.Get();
	return nullptr;
}

bool TextureManager::Contains(const std::wstring& name)
{
	return textures.find(name) != textures.end();
}

void TextureManager::ReleaseAll()
{
	textures.clear();
}