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

PSInput_PC main(VSInput_PC input)
{

    PSInput_PC output;
    float4 pos = float4(input.pos, 1.0f);
    pos = mul(pos, model);
    pos = mul(pos, viewProj);
    
    output.pos = pos;
    output.color = input.color;

    return output;
}
