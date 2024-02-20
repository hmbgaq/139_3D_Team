#include "Shader_Defines.hlsli"

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_SpecularTexture;
Texture2D g_EmissiveTexture;
Texture2D g_MRMaskTexture;

void ComputeNormalMapping(inout float4 normal, float3 tangent, float2 texcoord)
{
	// [0,255] 범위에서 [0,1]로 변환
    float4 map = g_NormalTexture.Sample(LinearSampler, texcoord);
    if (any(map.rgb) == false)
        return;

    float3 N = normalize(normal.xyz); // z
    float3 T = normalize(tangent); // x
    float3 B = normalize(cross(N, T)); // y
    float3x3 TBN = float3x3(T, B, N); // TS -> WS

	// [0,1] 범위에서 [-1,1] 범위로 변환
    float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
    float3 worldNormal = mul(tangentSpaceNormal, TBN);

    normal = float4(worldNormal, 0.f);
};
