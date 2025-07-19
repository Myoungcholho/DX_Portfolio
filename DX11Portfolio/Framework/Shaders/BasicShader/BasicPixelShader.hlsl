#include "../Common.hlsli"

TextureCube g_diffuseCube : register(t0);
TextureCube g_specularCube : register(t1);

Texture2D g_albedoTexture : register(t2);
Texture2D g_normalTexture : register(t3);
Texture2D g_aoTexture : register(t4);

SamplerState g_sampler : register(s0);

cbuffer CameraConstantData : register(b0)
{
    float3 eyeWorld;
    float mipmapLevel;
};

cbuffer MaterialConstantData : register(b2)
{
    LegacyMaterial material;
};

cbuffer LightConstantData : register(b3)
{
    Light light[MAX_LIGHTS];
    int lightCount;
    float3 padding;
};

cbuffer RenderOptionsConstantData : register(b4)
{
    int useAlbedoTexture;
    int useNormalMap;
    int useAOMap;
    int reverseNormalMapY;
};

float3 SchlickFresnel(float3 fresnelR0, float3 normal, float3 toEye)
{
    float normalDotView = saturate(dot(normal, toEye));
    float f0 = 1.0f - normalDotView; // 90도이면 f0 = 1, 0도이면 f0 = 0
    return fresnelR0 + (1.0f - fresnelR0) * pow(f0, 5.0);
}

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
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

PixelShaderOutput main(PixelShaderInput input)
{
    float3 toEye = normalize(eyeWorld - input.posWorld);
    float dist = length(eyeWorld - input.posWorld);
    float distMin = 3.0;
    float distMax = 15.0;
    float lod = 10.0 * saturate((dist - distMin) / (distMax - distMin));
    
    float3 color = float3(0.0, 0.0, 0.0);
    
    float3 normalWorld = input.normalWorld;
    
    // 노말 맵 사용 시
    if(useNormalMap)
    {
        float3 normalTex = g_normalTexture.SampleLevel(g_sampler, input.texcoord, lod).rgb;
        normalTex = 2.0 * normalTex - 1.0;
        
        normalTex.y = reverseNormalMapY ? -normalTex.y : normalTex.y;

        float3 N = normalWorld;
        float3 T = normalize(input.tangentWorld - dot(input.tangentWorld, N) * N);
        float3 B = cross(N, T);
        
        float3x3 TBN = float3x3(T, B, N);
        normalWorld = normalize(mul(normalTex, TBN));
    }
    
    // Forward Rendering
    int i = 0;
    [loop]
    for (i = 0; i < lightCount; ++i)
    {
        // Directional
        if(light[i].type == 0) // Directional
        {
            color += ComputeDirectionalLight(light[i], material, normalWorld, toEye);
        }
        else if(light[i].type ==1) //Point
        {
            color += ComputePointLight(light[i], material, input.posWorld, normalWorld, toEye);
        }
        else if(light[i].type == 2) //Spot
        {
            color += ComputeSpotLight(light[i], material, input.posWorld, normalWorld, toEye);
        }
    }
    
    // 
    float4 diffuse = g_diffuseCube.Sample(g_sampler, normalWorld) + float4(color, 1.0);
    float4 specular = g_specularCube.Sample(g_sampler, reflect(-toEye, normalWorld));
    
    diffuse *= float4(material.diffuse, 1.0);
    specular *= pow(abs(specular.r + specular.g + specular.b) / 3.0, material.shininess);
    specular *= float4(material.specular, 1.0);
    
    float3 f = SchlickFresnel(material.fresnelR0, normalWorld, toEye);
    specular.xyz *= f;
    
    if(useAlbedoTexture)
    {
        diffuse *= g_albedoTexture.SampleLevel(g_sampler, input.texcoord, lod);
        
        // float3 albedo = g_albedoTexture.SampleLevel(g_sampler, input.texcoord, lod).rgb;
        // diffuse.xyz = LinearToneMapping(albedo);
    }
    
    if(useAOMap)
        diffuse *= g_aoTexture.SampleLevel(g_sampler, input.texcoord, lod);
    
    PixelShaderOutput output;
    output.pixelColor = diffuse + specular;
    
    return output;
}