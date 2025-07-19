#pragma once

#include "Systems/IExecutable.h"

#include <directxtk/SimpleMath.h>
#include <vector>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class MeshGroup : public IExecutable
{
public:
	MeshGroup(std::string basePath, std::string filename);

public:
	virtual void Initialize();
	virtual void Destroy();

	virtual void UpdateGUI();
	virtual void Tick();
	virtual void Render();

private:
	void NormalInit(vector<PBRMeshData>& meshes, vector<D3D11_INPUT_ELEMENT_DESC>& basicInputElements);

private:
	// Shader
	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;

private:
	// Vertex와 Index 버퍼를 가지고, Texture, SRV 가짐
	// MipMap 생성을 위해 임시로 저장
	vector<shared_ptr<Mesh>> m_meshes;

private:
	// ConstantData
	//WorldInvConstantBuffer worldInvConstantBufferData;
	//CameraConstantBuffer cameraConstantBufferData;
	//MaterialConstantBuffer materialConstantBufferData;
	//LightConstantBuffer lightConstantBufferData;
	//RimParamsConstantBuffer rimParamsConstantBufferData;
	WorldInvConstantBuffer worldInvConstantBufferData;
	LightConstantBuffer lightConstantBufferData;
	
	BasicVertexConstantBuffer m_basicVertexConstData;
	BasicPixelConstantBuffer m_basicPixelConstData;


	// ConstnatBuffer
	//ComPtr<ID3D11Buffer> worldInvConstantBuffer;
	//ComPtr<ID3D11Buffer> cameraConstantBuffer;
	//ComPtr<ID3D11Buffer> materialConstantBuffer;
	//ComPtr<ID3D11Buffer> lightConstantBuffer;
	//ComPtr<ID3D11Buffer> rimParamsConstantBuffer;
	ComPtr<ID3D11Buffer> worldInvConstantBuffer;
	ComPtr<ID3D11Buffer> lightConstantBuffer;

	ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
	ComPtr<ID3D11Buffer> m_pixelConstantBuffer;

private:
	ComPtr<ID3D11InputLayout> m_inputLayout;

private:
	shared_ptr<Transform> transform;

private:
	float MaterialDiffuse = 0.8f;
	float MaterialSpecular = 1.0f;

	string Name = "Player";

private:
	string basePath;
	string filename;

private:
	// Normal용 정보
	shared_ptr<Mesh> m_normalLines;
	ComPtr<ID3D11VertexShader> m_normalVertexShader;
	ComPtr<ID3D11GeometryShader> m_normalGeometryShdaer;
	ComPtr<ID3D11PixelShader> m_normalPixelShader;
	bool m_drawNormalsDirtyFlag = true;
	bool m_drawNormals = false;
	NormalVertexConstantBuffer normalVertexConstantBufferData;
	ComPtr<ID3D11Buffer> normalVertexConstantBuffer;
	ComPtr<ID3D11InputLayout> m_inputLayoutNormal;
};