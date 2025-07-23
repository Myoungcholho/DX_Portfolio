#pragma once

#include "Systems/IExecutable.h"

#include <directxtk/SimpleMath.h>
#include <vector>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class CubeMapSkyRenderer
{
public:
	void Initialize();
	void UpdateGUI();
	void Tick();
	void Render(const bool &mirror);


private:
	CubeMapping m_cubeMapping;

private:
	WorldInvConstantBuffer worldInvConstantBufferData;
	ViewProjectionConstantBuffer viewProjectionData;
	MirrorViewProjectionConstantBuffer mirrorViewProjectionData;
	CubeMappingConstantBuffer cubeMappingConstantBufferData;

private:
	ComPtr<ID3D11Buffer> ViewProjectionBuffer;
	ComPtr<ID3D11Buffer> mirrorViewProjectionBuffer;
	ComPtr<ID3D11Buffer> cubeMappingConstantBuffer;


private:
	shared_ptr<Transform> transform;

private:
	wstring path = L"../../../_CubeMapTexture/HDRI/MorningSkyHalf/";
	wstring EnvHDR_Name= path + L"MorningSkyHalfCubeEnvHDR.dds";
	wstring Specular_Name = path + L"MorningSkyHalfCubeSpecularHDR.dds";
	wstring DiffuseHDR_Name = path + L"MorningSkyHalfCubeDiffuseHDR.dds";
	wstring Brdf_Name = path + L"MorningSkyHalfCubeBrdf.dds";
};