#pragma once

#include "Systems/IExecutable.h"

#include <directxtk/SimpleMath.h>
#include <vector>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class CubeMapSkyRenderer : public IExecutable
{
public:
	virtual void Initialize();
	virtual void Destroy();

	virtual void UpdateGUI();
	virtual void Tick();
	virtual void Render();

private:
	void CreateCubemapTexture(const wchar_t* filename, ComPtr<ID3D11ShaderResourceView>& textureResourceView);

private:
	CubeMapping m_cubeMapping;
	WorldInvConstantBuffer WorldMatrixData;

	ViewProjectionConstantBuffer ViewProjectionData;
	ComPtr<ID3D11Buffer> ViewProjectionBuffer;

	Vector3 ModelTranslation = Vector3(0.0f);
	Vector3 ModelRotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 ModelScaling = Vector3(1.8f);

private:
	wstring SkyBoxFilename = L"../../../_CubeMapTexture/BackGround.dds";
	wstring SkyBoxDiffuseFilename = L"../../../_CubeMapTexture/BackGroundBGRA_Diffuse.dds";
	wstring SkyBoxSpecularFilename = L"../../../_CubeMapTexture/BackGroundBGRA_Specular.dds";
	
	//wstring SkyBoxFilename = L"C:/DirectX/Portfolio/_CubeMapTexture/BackGround.dds";
	//wstring SkyBoxDiffuseFilename = L"C:/DirectX/Portfolio/_CubeMapTexture/BackGroundBGRA_Diffuse.dds";
	//wstring SkyBoxSpecularFilename = L"C:/DirectX/Portfolio/_CubeMapTexture/BackGroundBGRA_Specular.dds";
};