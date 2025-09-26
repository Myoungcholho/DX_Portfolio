#pragma once

template<typename T_ELEMENT>
class StructuredBuffer
{
public:
    virtual void Initialize(ID3D11Device* device, const UINT numElements)
    {
        m_cpu.resize(numElements);
        Initialize(device);
    }

    virtual void Initialize(ID3D11Device* device)
    {
        D3D11Utils::CreateStructuredBuffer(device, UINT(m_cpu.size()), sizeof(T_ELEMENT), m_cpu.data(), m_gpu, m_srv, m_uav);
        
        // Staging은 주로 디버깅 용도
        D3D11Utils::CreateStagingBuffer(device, UINT(m_cpu.size()), sizeof(T_ELEMENT), NULL, m_staging);
    }

    void Upload(ID3D11DeviceContext* context)
    {
        Upload(context, m_cpu);
    }

    void Upload(ID3D11DeviceContext* context, vector<T_ELEMENT>& arrCpu)
    {
        assert(arrCpu.size() == m_cpu.size());

        D3D11Utils::CopyToStagingBuffer(context, m_staging, UINT(arrCpu.size() * sizeof(T_ELEMENT)), arrCpu.data());
        context->CopyResource(m_gpu.Get(), m_staging.Get());
    }

    void Download(ID3D11DeviceContext* context)
    {
        Download(context, m_cpu);
    }

    void Download(ID3D11DeviceContext* context, vector<T_ELEMENT>& arrCpu)
    {
        assert(arrCpu.size() == m_cpu.size());

        context->CopyResource(m_staging.Get(), m_gpu.Get());
        D3D11Utils::CopyFromStagingBuffer(context, m_staging, UINT(arrCpu.size() * sizeof(T_ELEMENT)),arrCpu.data());
    }

    const auto GetBuffer() { return m_gpu.Get(); }
    const auto GetSRV() { return m_srv.Get(); }
    const auto GetUAV() { return m_uav.Get(); }
    const auto GetAddressOfSRV() { return m_srv.GetAddressOf(); }
    const auto GetAddressOfUAV() { return m_uav.GetAddressOf(); }
    const auto GetStaging() { return m_staging.Get(); }

    vector<T_ELEMENT> m_cpu;                        // cpu 데이터
    ComPtr<ID3D11Buffer> m_gpu;                     // gpu 데이터
    ComPtr<ID3D11Buffer> m_staging;                 // CPU에 복사용

    ComPtr<ID3D11ShaderResourceView> m_srv;
    ComPtr<ID3D11UnorderedAccessView> m_uav;
};