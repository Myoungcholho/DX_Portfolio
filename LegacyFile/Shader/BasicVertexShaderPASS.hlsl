struct VertexShaderInput
{
    float3 posModel : POSITION;
    float3 normalModel : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

struct HullShaderInput
{
    float3 posModel : POSITION;
    float3 normalModel : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

HullShaderInput main(VertexShaderInput input)
{
    HullShaderInput output;
    output.posModel = input.posModel;
    output.normalModel = input.normalModel;
    output.texcoord = input.texcoord;
    output.tangentModel = input.tangentModel;
    
    return output;
}