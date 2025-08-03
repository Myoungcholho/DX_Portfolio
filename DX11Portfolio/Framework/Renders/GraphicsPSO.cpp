#include "Framework.h"
#include "GraphicsPSO.h"

void GraphicsPSO::operator=(const GraphicsPSO& pso)
{
    m_vertexShader = pso.m_vertexShader;
    m_pixelShader = pso.m_pixelShader;
    m_hullShader = pso.m_hullShader;
    m_domainShader = pso.m_domainShader;
    m_geometryShader = pso.m_geometryShader;
    m_inputLayout = pso.m_inputLayout;
    m_blendState = pso.m_blendState;
    m_depthStencilState = pso.m_depthStencilState;
    m_rasterizerState = pso.m_rasterizerState;
    m_stencilRef = pso.m_stencilRef;
    for (int i = 0; i < 4; i++)
        m_blendFactor[i] = pso.m_blendFactor[i];
    m_primitiveTopology = pso.m_primitiveTopology;
}

void GraphicsPSO::SetBlendFactor(const float blendFactor[4])
{
    memcpy(m_blendFactor, blendFactor, sizeof(float) * 4);
}

void GraphicsPSO::Apply(ID3D11DeviceContext* context) const
{
    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->PSSetShader(m_pixelShader.Get(), 0, 0);
    context->HSSetShader(m_hullShader.Get(), 0, 0);
    context->DSSetShader(m_domainShader.Get(), 0, 0);
    context->GSSetShader(m_geometryShader.Get(), 0, 0);
    context->IASetInputLayout(m_inputLayout.Get());
    context->RSSetState(m_rasterizerState.Get());
    context->OMSetBlendState(m_blendState.Get(), m_blendFactor,0xffffffff);
    context->OMSetDepthStencilState(m_depthStencilState.Get(),m_stencilRef);
    context->IASetPrimitiveTopology(m_primitiveTopology);
}