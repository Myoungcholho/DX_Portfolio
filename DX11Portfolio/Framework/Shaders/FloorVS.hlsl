#include "VertexStructures.hlsli"

cbuffer ModelBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
};

cbuffer CameraBuffer : register(b1)
{
    matrix viewProj;
};

PSInput_PNCT main(VSInput_PNCT input)
{
    PSInput_PNCT output;
    float4 pos = float4(input.pos, 1.0f);
    pos = mul(pos, model);
    pos = mul(pos, viewProj);
    
    output.pos = pos;
    output.normal = input.normal;
    output.color = input.color;
    output.uv = input.uv;
    
    return output;
}