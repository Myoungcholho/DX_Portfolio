Texture2D g_heightTexture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer WorldMatrixConstantBuffer : register(b0)
{
    Matrix world;
    Matrix invTranspose;
};

cbuffer ViewProjConstantBuffer : register(b1)
{
    Matrix viewProj;
};

cbuffer HeightMapConstantBuffer : register(b2)
{
    int useHeightMap;
    float heightScale = 0.0f;
    float2 dummy;
};

struct VertexShaderInput
{
    float3 posModel : POSITION; 
    float3 normalModel : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (조명 계산에 사용)
    float3 normalWorld : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentWorld : TANGENT0;
    float3 color : COLOR; // Normal lines 쉐이더에서 사용
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    
    // Normal
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, invTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    // Tangent
    float4 tangentWorld = float4(input.tangentModel, 0.0f);
    tangentWorld = mul(tangentWorld, world);
    
    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, world);
    
    if(useHeightMap)
    {
        float height = g_heightTexture.SampleLevel(g_sampler, input.texcoord, 0).r;
        height = height * 2.0 - 1.0;
        pos += float4(output.normalWorld * height * heightScale, 0.0);
    }
    
    output.posWorld = pos.xyz;
    
    pos = mul(pos, viewProj);
    
    output.posProj = pos;
    output.texcoord = input.texcoord;
    output.tangentWorld = tangentWorld.xyz;
    
    output.color = float3(0.0f, 0.0f, 0.0f);

    return output;
}