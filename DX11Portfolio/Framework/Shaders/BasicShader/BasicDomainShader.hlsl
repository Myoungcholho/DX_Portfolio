Texture2D g_heightTexture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer WorldMatrixConstantBuffer : register(b0)
{
    matrix world;
    matrix invTranspose;
};

cbuffer ViewProjConstantBuffer : register(b1)
{
    matrix view;
    matrix projection;
};

cbuffer HeightMapConstantBuffer : register(b2)
{
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

struct HullShaderOutput
{
    float3 posModel : POSITION;
    float3 normalModel : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

struct PixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posWorld : POSITION;
    float3 normalWorld : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentWorld : TANGENT0;
    float3 color : COLOR;
};

struct PatchConstant
{
    float edgeTess[4] : SV_TessFactor;
    float insideTess[2] : SV_InsideTessFactor;
};

[domain("quad")]
PixelShaderInput main(
    const OutputPatch<HullShaderOutput, 4> patch,
    float2 uv : SV_DomainLocation,
    const PatchConstant patchConst)
{
    PixelShaderInput output;

    // 보간된 모델공간 속성
    float3 posModel = patch[0].posModel * (1 - uv.x) * (1 - uv.y) +
                      patch[1].posModel * uv.x * (1 - uv.y) +
                      patch[2].posModel * (1 - uv.x) * uv.y +
                      patch[3].posModel * uv.x * uv.y;

    float3 normalModel = patch[0].normalModel * (1 - uv.x) * (1 - uv.y) +
    patch[1].normalModel * uv.x * (1 - uv.y) +
    patch[2].normalModel * (1 - uv.x) * uv.y +
    patch[3].normalModel * uv.x * uv.y;

    float2 texcoord = patch[0].texcoord * (1 - uv.x) * (1 - uv.y) +
                      patch[1].texcoord * uv.x * (1 - uv.y) +
                      patch[2].texcoord * (1 - uv.x) * uv.y +
                      patch[3].texcoord * uv.x * uv.y;

    float3 tangentModel = patch[0].tangentModel * (1 - uv.x) * (1 - uv.y) +
                      patch[1].tangentModel * uv.x * (1 - uv.y) +
                      patch[2].tangentModel * (1 - uv.x) * uv.y +
                      patch[3].tangentModel * uv.x * uv.y;

    // 월드 좌표계로 변환
    float4 normal = float4(normalModel, 0.0f);
    output.normalWorld = mul(normal, invTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);

    float4 tangent = float4(tangentModel, 0.0f);
    output.tangentWorld = mul(tangent, world).xyz;
    output.tangentWorld = normalize(output.tangentWorld);
    
    float4 posWorld = mul(float4(posModel, 1.0f), world);

    if (useHeightMap)
    {
        float height = g_heightTexture.SampleLevel(g_sampler, texcoord, 0).r;
        height = height * 2.0f - 1.0f;
        posWorld.xyz += output.normalWorld * height * heightScale;
    }

    output.posWorld = posWorld.xyz;

    float4 viewPos = mul(posWorld, view);
    output.posProj = mul(viewPos, projection);

    output.texcoord = texcoord;
    output.color = float3(0, 0, 0);

    return output;
}