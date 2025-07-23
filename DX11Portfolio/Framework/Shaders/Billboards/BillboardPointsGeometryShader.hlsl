cbuffer BillboardPointsConstantData : register(b0)
{
    float3 eyeWorld;
    float width;
}

cbuffer CameraBuffer : register(b1)
{
    matrix viewProj;
};

cbuffer WorldBuffer : register(b2)
{
    matrix world;
};

struct GeometryShaderInput
{
    float4 pos : SV_POSITION;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION; // not POSITION
    float2 texCoord : TEXCOORD; // 텍스춰 좌표 추가
    uint primID : SV_PrimitiveID;
};

// CCW로 해야 보이는데 문제가 뭘까?
[maxvertexcount(4)]
void main(point GeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              inout TriangleStream<PixelShaderInput> outputStream)
{
    float hw = 0.5 * width;
    
    float4 up = float4(0.0, 1.0, 0.0, 0.0);
    float4 front = float4(eyeWorld.xyz, 1.0) - input[0].pos;
    front.w = 0.0;
    float4 right = float4(cross(up.xyz, normalize(front.xyz)), 0.0);
    
    PixelShaderInput output;
    
    // 왼쪽 하단
    output.pos = input[0].pos - hw * right - hw * up;
    output.pos = mul(output.pos, viewProj);
    output.texCoord = float2(0.0, 1.0);
    output.primID = primID;
    
    outputStream.Append(output);

        // 오른쪽 하단
    output.pos = input[0].pos + hw * right - hw * up;
    output.pos = mul(output.pos, viewProj);
    output.texCoord = float2(1.0, 1.0);
    output.primID = primID;
    
    outputStream.Append(output);
    
    // 왼쪽 상단
    output.pos = input[0].pos - hw * right + hw * up;
    output.pos = mul(output.pos, viewProj);
    output.texCoord = float2(0.0, 0.0);
    output.primID = primID;
    
    outputStream.Append(output);
   
    
    // 오른쪽 상단
    output.pos = input[0].pos + hw * right + hw * up;
    output.pos = mul(output.pos, viewProj);
    output.texCoord = float2(1.0, 0.0);
    output.primID = primID;
    
    outputStream.Append(output);
}