#pragma once

#include "Systems/IExecutable.h"

#include <directxtk/SimpleMath.h>
#include <vector>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class TestObjectModel : public IExecutable
{
public:
	virtual void Initialize();
	virtual void Destroy();

	virtual void UpdateGUI();
	virtual void Tick();
	virtual void Render();

private:
	void NormalInit(vector<MeshData>& meshes, vector<D3D11_INPUT_ELEMENT_DESC>& basicInputElements);

	shared_ptr<FMesh> m_normalLines;
	ComPtr<ID3D11VertexShader> m_normalVertexShader;
	ComPtr<ID3D11GeometryShader> m_normalGeometryShdaer;
	ComPtr<ID3D11PixelShader> m_normalPixelShader;

	bool m_drawNormals = false;
private:
    ComPtr<ID3D11VertexShader> BasicVertexShader;
    ComPtr<ID3D11PixelShader> BasicPixelShader;
    ComPtr<ID3D11InputLayout> InputLayout;

    vector<shared_ptr<FMesh>> Meshs;

    WorldInvConstantBuffer WorldMatrixData;
	BasicPixelConstantBuffer BasicPixelConstantBufferData;

	// 오브젝트 별로 들고 있을 RimLight 정보
	RimParamsCBuffer RimParamsData;
	ComPtr<ID3D11Buffer> RimParamsCBuffer;

	/*Vector3 ModelScaling = Vector3(1.8f);
	Vector3 ModelRotation = Vector3(0.0f,0.0f, 0.0f);
	Vector3 ModelTranslation = Vector3(4.0f,-3.f,10.0f);*/

	shared_ptr<Transform> transform;

	float MaterialDiffuse = 0.8f;
	float MaterialSpecular = 1.0f;

	string Name = "Player";
	
};