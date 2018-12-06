#include "Cube.hlsli"

// ÏñËØ×ÅÉ«Æ÷(2D)
float4 PS_2D_COLOR(VertexOut2D pIn) : SV_Target
{
    float4 color = pIn.Color.xyzw;
    clip(color.a - 0.1f);
    return color;
}