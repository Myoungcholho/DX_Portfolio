#pragma once

#include <Windows.h>
#include <stdexcept>

using Microsoft::WRL::ComPtr;
using std::shared_ptr;
using std::vector;
using std::wstring;

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

class D3D11Utils
{
public:
    static void CreateVertexShaderAndInputLayout
    (
        ComPtr<ID3D11Device>& device, const wstring& filename,
        const vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
        ComPtr<ID3D11VertexShader>& m_vertexShader,
        ComPtr<ID3D11InputLayout>& m_inputLayout
    );



};