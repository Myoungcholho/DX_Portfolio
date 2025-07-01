#pragma once

#include <unordered_map>

using Microsoft::WRL::ComPtr;

class TextureManager
{
public:
	void Initialize();
	void Register(const std::wstring& name, ID3D11ShaderResourceView* srv);
	void UnRegister(const std::wstring& name);
	ID3D11ShaderResourceView* Get(const std::wstring& name) const;
	bool Contains(const std::wstring& name);
	void ReleaseAll();

private:
	// 
	std::unordered_map<std::wstring, ComPtr<ID3D11ShaderResourceView>> textures;
};