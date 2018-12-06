#include "Cube.hlsli"

// ������ɫ��(2D)
VertexOut VS_2D(VertexIn pIn)
{
    VertexOut pOut;
    pOut.PosH = float4(pIn.PosL, 1.0f);
    pOut.PosW = float3(0.0f, 0.0f, 0.0f);
    pOut.NormalW = pIn.NormalL;
    pOut.Tex = mul(float4(pIn.Tex, 0.0f, 1.0f), gTexTransform).xy;
    return pOut;
}