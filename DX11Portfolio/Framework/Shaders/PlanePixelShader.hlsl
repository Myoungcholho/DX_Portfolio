#include "VertexStructures.hlsli"


float4 main(PSInput_PC input) : SV_TARGET
{
    return float4(input.color, 1.0);
}
