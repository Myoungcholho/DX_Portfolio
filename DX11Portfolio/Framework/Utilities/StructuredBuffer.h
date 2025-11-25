#pragma once

template<typename T_ELEMENT>
class StructuredBuffer
{
public:
    virtual void Initialize(ID3D11Device* device, const UINT numElements)
    {
        cpuData.resize(numElements);
        Initialize(device);
    }

    virtual void Initialize(ID3D11Device* device)
    {
        D3D11Utils::CreateStructuredBuffer(device, UINT(cpuData.size()), sizeof(T_ELEMENT), cpuData.data(), gpuData, srv, uav);
        
        // Staging은 주로 디버깅 용도
        D3D11Utils::CreateStagingBuffer(device, UINT(cpuData.size()), sizeof(T_ELEMENT), NULL, stagingData);
    }

    void Upload(ID3D11DeviceContext* context)
    {
        Upload(context, cpuData);
    }

    void Upload(ID3D11DeviceContext* context, vector<T_ELEMENT>& arrCpu)
    {
        assert(arrCpu.size() == cpuData.size());

        D3D11Utils::CopyToStagingBuffer(context, stagingData, UINT(arrCpu.size() * sizeof(T_ELEMENT)), arrCpu.data());
        context->CopyResource(gpuData.Get(), stagingData.Get());
    }

    void Download(ID3D11DeviceContext* context)
    {
        Download(context, cpuData);
    }

    void Download(ID3D11DeviceContext* context, vector<T_ELEMENT>& arrCpu)
    {
        assert(arrCpu.size() == cpuData.size());

        context->CopyResource(stagingData.Get(), gpuData.Get());
        D3D11Utils::CopyFromStagingBuffer(context, stagingData, UINT(arrCpu.size() * sizeof(T_ELEMENT)),arrCpu.data());
    }

    const auto GetBuffer() { return gpuData.Get(); }
    const auto GetSRV() { return srv.Get(); }
    const auto GetUAV() { return uav.Get(); }
    const auto GetAddressOfSRV() { return srv.GetAddressOf(); }
    const auto GetAddressOfUAV() { return uav.GetAddressOf(); }
    const auto GetStaging() { return stagingData.Get(); }

    vector<T_ELEMENT> cpuData;                        // cpu 데이터
    ComPtr<ID3D11Buffer> gpuData;                     // gpu 데이터
    ComPtr<ID3D11Buffer> stagingData;                 // CPU에 복사용

    ComPtr<ID3D11ShaderResourceView> srv;
    ComPtr<ID3D11UnorderedAccessView> uav;
};