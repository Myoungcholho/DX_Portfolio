#include "../Common.hlsli" // 쉐이더에서도 include 사용 가능

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D aoTex : register(t2);
Texture2D metallicRoughnessTex : register(t3);
Texture2D emissiveTex : register(t4);

// StructuredBuffer에서 인스턴스 데이터 가져오기
StructuredBuffer<InstanceData> gInstances : register(t8);

static const float3 Fdielectric = 0.04;

// Fresnel
float3 SchlickFresnel(float3 F0, float NdotH)
{
    return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * NdotH - 6.98316) * NdotH);
}

// 출력
struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

float3 GetNormal(PixelShaderInputInstance input, InstanceMaterial mat)
{
    float3 normalWorld = input.normalWorld;
    
    if (mat.useNormalMap) // NormalWorld 교체
    {
        float3 normal = normalTex.Sample(anisoWrapSampler, input.texcoord).rgb;
        normal = 2.0 * normal - 1.0;

        // OpenGL 계열 normal map은 Y 플립
        normal.y = mat.invertNormalMapY ? -normal.y : normal.y;
        
        float3 N = normalWorld;
        float3 T = normalize(input.tangentWorld - dot(input.tangentWorld, N) * N);
        float3 B = cross(N, T);
        
        float3x3 TBN = float3x3(T, B, N);
        normalWorld = normalize(mul(normal, TBN));
    }
    
    return normalWorld;
}

// ------PBR의 IBL 계산------

float3 DiffuseIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                  float metallic)
{
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnel(F0, max(0.0, dot(normalWorld, pixelToEye)));
    float3 kd = lerp(1.0 - F, 0.0, metallic);
    float3 irradiance = irradianceIBLTex.SampleLevel(linearWrapSampler, normalWorld, 0).rgb;
    
    return kd * (albedo / 3.141592) * irradiance;
}

float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                   float metallic, float roughness)
{
    float2 specularBRDF = brdfTex.SampleLevel(linearClampSampler, float2(dot(normalWorld, pixelToEye), 1.0 - roughness), 0.0f).rg;
    float3 specularIrradiance = specularIBLTex.SampleLevel(linearWrapSampler, reflect(-pixelToEye, normalWorld),2 + roughness * 5.0f).rgb;
    const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0
    float3 F0 = lerp(Fdielectric, albedo, metallic);

    return (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
}

float3 AmbientLightingByIBL(float3 albedo, float3 normalW, float3 pixelToEye, float ao,
                            float metallic, float roughness)
{
    float3 diffuseIBL = DiffuseIBL(albedo, normalW, pixelToEye, metallic);
    float3 specularIBL = SpecularIBL(albedo, normalW, pixelToEye, metallic, roughness);
    
    return (diffuseIBL + specularIBL) * ao;
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float NdfGGX(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;

    return alphaSq / (3.141592 * denom * denom);
}

// Single term for separable Schlick-GGX below.
float SchlickG1(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float SchlickGGX(float NdotI, float NdotO, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return SchlickG1(NdotI, k) * SchlickG1(NdotO, k);
}

float3 LightRadiance(Light light, float3 posWorld, float3 normalWorld, Texture2D shadowMap)
{
    float3 L;
    float att;
    if (light.type & LIGHT_DIRECTIONAL)
    {
        L = normalize(-light.direction);
        att = 1.0;
    }
    else
    {
        float3 toLight = light.position - posWorld;
        float dist = length(toLight);
        L = toLight / max(dist, 1e-6);
        att = saturate((light.fallOffEnd - dist) / (light.fallOffEnd - light.fallOffStart));
    }

    float spotFactor = (light.type & LIGHT_SPOT) ? pow(saturate(-dot(L, light.direction)), light.spotPower) : 1.0;

    // Shadow map
    float shadowFactor = 1.0;

    if (light.type & LIGHT_SHADOW)
    {
        const float nearZ = 0.01; // 카메라 설정과 동일
        
        // 1. Project posWorld to light screen
        float4 lightScreen = mul(float4(posWorld, 1.0), light.viewProj);
        lightScreen.xyz /= lightScreen.w;
        
        float2 lightTexcoord = float2(lightScreen.x, -lightScreen.y);
        lightTexcoord += 1.0;
        lightTexcoord *= 0.5;
        
        //float depth = shadowMap.Sample(shadowPointSampler, lightTexcoord).r;
        
        //// ShadowMap < 빛기준으로 봤을때 Depth비교(지면)
        //if (depth + 0.00001f < lightScreen.z)
        //    shadowFactor = 0.0;
        
        //shadowFactor = shadowMap.SampleCmpLevelZero(shadowCompareSampler, lightTexcoord.xy, lightScreen.z - 0.00001f).r;
        
        uint width, height, numMips;
        shadowMap.GetDimensions(0, width, height, numMips);
        float dx = 1.0 / (float) width;
        float percentLit = 0.0;
        const float2 offsets[9] =
        {
            float2(-1, -1), float2(0, -1), float2(+1, -1),
            float2(-1, 0), float2(0, 0), float2(+1, 0),
            float2(-1, +1), float2(0, +1), float2(1, +1)
        };

        [unroll]
        for (int i = 0; i < 9; ++i)
        {
            percentLit += shadowMap.SampleCmpLevelZero(shadowCompareSampler, lightTexcoord.xy + offsets[i] * dx, lightScreen.z - 0.00001f).r;
        }
        
        shadowFactor = percentLit / 9.0;

    }

    float3 radiance = light.radiance * spotFactor * att * shadowFactor;

    return radiance;
}

PixelShaderOutput main(PixelShaderInputInstance input)
{
    // 현재 인스턴스의 머티리얼 데이터
    InstanceMaterial mat = gInstances[input.instanced].mat;

    float3 pixelToEye = normalize(eyeWorld - input.posWorld);
    float3 normalWorld = GetNormal(input, mat);

    float3 albedo = mat.useAlbedoMap
        ? albedoTex.Sample(anisoWrapSampler, input.texcoord).rgb * mat.albedoFactor
        : mat.albedoFactor;

    float ao = mat.useAOMap
        ? aoTex.Sample(anisoWrapSampler, input.texcoord).r
        : 1.0;

    float metallic = mat.useMetallicMap
        ? metallicRoughnessTex.Sample(anisoWrapSampler, input.texcoord).b * mat.metallicFactor
        : mat.metallicFactor;

    float roughness = mat.useRoughnessMap
        ? metallicRoughnessTex.Sample(anisoWrapSampler, input.texcoord).g * mat.roughnessFactor
        : mat.roughnessFactor;

    float3 emission = mat.useEmissiveMap
        ? emissiveTex.Sample(anisoWrapSampler, input.texcoord).rgb
        : mat.emissionFactor;

    float3 ambientLighting =
        AmbientLightingByIBL(albedo, normalWorld, pixelToEye, ao, metallic, roughness) * strengthIBL;

    float3 directLighting = 0.0f;

    [unroll]
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        if (lights[i].enabled && lights[i].type)
        {
            float3 lightVec = lights[i].position - input.posWorld;
            float lightDist = length(lightVec);
            lightVec /= lightDist;

            float3 halfway = normalize(pixelToEye + lightVec);
        
            float NdotI = max(0.0, dot(normalWorld, lightVec));
            float NdotH = max(0.0, dot(normalWorld, halfway));
            float NdotO = max(0.0, dot(normalWorld, pixelToEye));
        
            float3 F0 = lerp(Fdielectric, albedo, metallic);
            float3 F = SchlickFresnel(F0, max(0.0, dot(halfway, pixelToEye)));
            float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
            float3 diffuseBRDF = kd * (albedo / 3.141592);

            float D = NdfGGX(NdotH, roughness);
            float3 G = SchlickGGX(NdotI, NdotO, roughness);
            float3 specularBRDF = (F * D * G) / max(1e-5, 4.0 * NdotI * NdotO);

            float3 radiance = 0.0f;
            
            radiance = LightRadiance(lights[i], input.posWorld, normalWorld, shadowMaps[i]);
                
            directLighting += (diffuseBRDF + specularBRDF) * radiance * NdotI;
        }
    }
    
    PixelShaderOutput output;
    output.pixelColor = float4(ambientLighting + directLighting + emission, 1.0);
    output.pixelColor = clamp(output.pixelColor, 0.0, 1000.0);
    
    return output;
}