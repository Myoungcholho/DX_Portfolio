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

struct GeometryShaderInput
{
    float4 posModel : SV_POSITION;
    float3 normalWorld : NORMAL;
};

GeometryShaderInput main(VSInput_PNT input)
{
    GeometryShaderInput output;
    
    output.posModel = float4(input.pos, 1.0);
    output.normalWorld = input.normal;
    
    return output;
}
