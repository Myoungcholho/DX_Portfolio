#include "../Common.hlsli"

struct SkyboxPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
};

PixelShaderOutput main(SkyboxPixelShaderInput input)
{
    PixelShaderOutput output;
    
    if (textureToDraw == 0)
        output.pixelColor = envIBLTex.Sample(linearWrapSampler, input.posModel.xyz);
    else if (textureToDraw == 1)
        output.pixelColor = specularIBLTex.Sample(linearWrapSampler, input.posModel.xyz);
    else if (textureToDraw == 2)
        output.pixelColor = irradianceIBLTex.Sample(linearWrapSampler, input.posModel.xyz);
    else
        output.pixelColor = float4(135.0 / 255.0, 206.0 / 255.0, 235.0 / 255.0, 1.0); // float literal로 정확히

    output.pixelColor *= strengthIBL;
    
    return output;
}
