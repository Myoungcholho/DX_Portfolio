cbuffer BillboardPointsConstantData : register(b0)
{
    float3 eyeWorld;
    float width;
}

cbuffer CameraBuffer : register(b1)
{
    matrix view;
    matrix projection;
};

cbuffer WorldBuffer : register(b2)
{
    matrix world;
};

struct VertexShaderInput
{
    float4 pos : POSITION; // ¸ðµ¨ ÁÂÇ¥°èÀÇ À§Ä¡ position
};

struct GeometryShaderInput
{
    float4 pos : SV_POSITION; // Screen position
};

GeometryShaderInput main(VertexShaderInput input)
{
    GeometryShaderInput output;
    
    output.pos = mul(input.pos, world);
    
    return output;
}