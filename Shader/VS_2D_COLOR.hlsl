#include "Cube.hlsli"

// ¶¥µã×ÅÉ«Æ÷(2D)
VertexOut2D VS_2D_COLOR(VertexIn2D pIn)
{
    VertexOut2D pOut;
    pOut.PosH = float4(pIn.PosL, 1.0f);
    pOut.PosW = float3(0.0f, 0.0f, 0.0f);
    pOut.NormalW = pIn.NormalL;
    pOut.Color = pIn.Color;
    return pOut;
}