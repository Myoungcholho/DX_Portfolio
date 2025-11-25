#include "Framework.h"
#include "GraphicsPSO.h"

void GraphicsPSO::operator=(const GraphicsPSO& pso)
{
    vertexShader = pso.vertexShader;
    pixelShader = pso.pixelShader;
    hullShader = pso.hullShader;
    domainShader = pso.domainShader;
    geometryShader = pso.geometryShader;
    inputLayout = pso.inputLayout;
    blendState = pso.blendState;
    depthStencilState = pso.depthStencilState;
    rasterizerState = pso.rasterizerState;
    stencilRef = pso.stencilRef;
    for (int i = 0; i < 4; i++)
        blendFactor[i] = pso.blendFactor[i];
    primitiveTopology = pso.primitiveTopology;
}

void GraphicsPSO::SetBlendFactor(const float InBlendFactor[4])
{
    memcpy(blendFactor, InBlendFactor, sizeof(float) * 4);
}

void GraphicsPSO::Apply(ID3D11DeviceContext* context) const
{
    context->VSSetShader(vertexShader.Get(), 0, 0);
    context->PSSetShader(pixelShader.Get(), 0, 0);
    context->HSSetShader(hullShader.Get(), 0, 0);
    context->DSSetShader(domainShader.Get(), 0, 0);
    context->GSSetShader(geometryShader.Get(), 0, 0);
    context->IASetInputLayout(inputLayout.Get());
    context->RSSetState(rasterizerState.Get());
    context->OMSetBlendState(blendState.Get(), blendFactor,0xffffffff);
    context->OMSetDepthStencilState(depthStencilState.Get(),stencilRef);
    context->IASetPrimitiveTopology(primitiveTopology);
}