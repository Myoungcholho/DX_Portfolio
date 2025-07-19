#include "Framework.h"
#include "PostProcess.h"

void PostProcess::Initialize(const std::vector<ComPtr<ID3D11ShaderResourceView>>& resources, const std::vector<ComPtr<ID3D11RenderTargetView>>& targets, const int width, const int height, const int bloomLevels)
{
	PBRMeshData meshData = GeomtryGenerator::MakeSquare();

	m_mesh = std::make_shared<Mesh>();

	D3D::Get()->CreateVertexBuffer(meshData.vertices, m_mesh->vertexBuffer);
	D3D::Get()->CreateIndexBuffer(meshData.indices, m_mesh->indexBuffer);
	m_mesh->m_indexCount = UINT(meshData.indices.size());

    vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = 
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D::Get()->CreateVertexShaderAndInputLayout(L"PostProcess/SamplingVS.hlsl", basicInputElements, m_vertexShader, m_inputLayout);
    D3D::Get()->CreatePixelShader(L"PostProcess/BloomDownPS.hlsl", m_bloomDownPixelShader);
    D3D::Get()->CreatePixelShader(L"PostProcess/BloomUpPS.hlsl", m_bloomUpPixelShader);
    D3D::Get()->CreatePixelShader(L"PostProcess/CombinePixelShader.hlsl",m_combinePixelShader);

    // Sampler는 외부에서
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    D3D::Get()->GetDevice()->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());

    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = false;
    D3D::Get()->GetDevice()->CreateRasterizerState(&rastDesc, m_rasterizerSate.GetAddressOf());

    m_bloomSRVs.resize(bloomLevels);
    m_bloomRTVs.resize(bloomLevels);

    // Bloom Down/Up
    m_bloomSRVs.resize(bloomLevels);
    m_bloomRTVs.resize(bloomLevels);
    for (int i = 0; i < bloomLevels; i++) 
    {
        int div = int(pow(2, i));
        CreateBuffer(width / div, height / div, m_bloomSRVs[i],m_bloomRTVs[i]);
    }

    m_bloomDownFilters.resize(bloomLevels - 1);
    for (int i = 0; i < bloomLevels - 1; i++) 
    {
        int div = int(pow(2, i + 1));
        m_bloomDownFilters[i].Initialize(m_bloomDownPixelShader, width / div, height / div);
        if (i == 0) 
        {
            m_bloomDownFilters[i].SetShaderResources({ resources[0] });
        }
        else 
        {
            m_bloomDownFilters[i].SetShaderResources({ m_bloomSRVs[i] });
        }

        m_bloomDownFilters[i].SetRenderTargets({ m_bloomRTVs[i + 1] });
    }

    m_bloomUpFilters.resize(bloomLevels - 1);
    for (int i = 0; i < bloomLevels - 1; i++) 
    {
        int level = bloomLevels - 2 - i;
        int div = int(pow(2, level));
        m_bloomUpFilters[i].Initialize(m_bloomUpPixelShader,width / div, height / div);
        m_bloomUpFilters[i].SetShaderResources({ m_bloomSRVs[level + 1] });
        m_bloomUpFilters[i].SetRenderTargets({ m_bloomRTVs[level] });
    }

    // Combine + ToneMapping
    m_combineFilter.Initialize(m_combinePixelShader, width,height);
    m_combineFilter.SetShaderResources({ resources[0], m_bloomSRVs[0] });
    m_combineFilter.SetRenderTargets(targets);
    m_combineFilter.m_constData.strength = 0.0f;
    m_combineFilter.m_constData.option1 = 1.0f;
    m_combineFilter.m_constData.option2 = 2.2f;
    m_combineFilter.UpdateConstantBuffers();
}

void PostProcess::UpdateGUI()
{
    ImGui::Begin("Post Process");
    {
        int flag = 0;
        flag += ImGui::SliderFloat("Bloom Strength",&m_combineFilter.m_constData.strength, 0.0f, 1.0f);
        flag += ImGui::SliderFloat("Exposure", &m_combineFilter.m_constData.option1,0.0f, 10.0f);
        flag += ImGui::SliderFloat("Gamma", &m_combineFilter.m_constData.option2, 0.1f, 5.0f);

        if (flag)
        {
            m_combineFilter.UpdateConstantBuffers();
        }
    }
    ImGui::End();
}

void PostProcess::Render()
{
    ID3D11DeviceContext* context = D3D::Get()->GetDeviceContext();

    context->RSSetState(m_rasterizerSate.Get());

    UINT stride = sizeof(FVertexPNTT);
    UINT offset = 0;

    context->IASetInputLayout(m_inputLayout.Get());
    context->IASetVertexBuffers(0, 1, m_mesh->vertexBuffer.GetAddressOf(),&stride, &offset);
    context->IASetIndexBuffer(m_mesh->indexBuffer.Get(),DXGI_FORMAT_R32_UINT,0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 
    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    for (int i = 0; i < m_bloomDownFilters.size(); i++) 
    {
        RenderImageFilter(m_bloomDownFilters[i]);
    }

    for (int i = 0; i < m_bloomUpFilters.size(); i++) 
    {
        RenderImageFilter(m_bloomUpFilters[i]);
    }

    RenderImageFilter(m_combineFilter);
}

void PostProcess::RenderImageFilter(const ImageFilter& imageFilter)
{
    imageFilter.Render();
    D3D::Get()->GetDeviceContext()->DrawIndexed(m_mesh->m_indexCount, 0, 0);
}

void PostProcess::CreateBuffer(int width, int height, ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11RenderTargetView>& rtv)
{
    ComPtr<ID3D11Texture2D> texture;

    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = txtDesc.ArraySize = 1;
    txtDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; //  이미지 처리용도
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_DEFAULT;
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    txtDesc.MiscFlags = 0;
    txtDesc.CPUAccessFlags = 0;

    ID3D11Device* device = D3D::Get()->GetDevice();

    ThrowIfFailed(device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf()));
    ThrowIfFailed(device->CreateRenderTargetView(texture.Get(), NULL,rtv.GetAddressOf()));
    ThrowIfFailed(device->CreateShaderResourceView(texture.Get(), NULL,srv.GetAddressOf()));
}