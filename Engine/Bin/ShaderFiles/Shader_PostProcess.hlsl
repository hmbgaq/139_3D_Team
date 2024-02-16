#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* ssao */
vector  g_OffsetVector[26];
vector  FrustumCorner[4];
Texture2D NormalDepthMap;
Texture2D RandomVecMap;

matrix  ViewToTexSpcace;

/* godray */


/* ------------------- Vertex Shader(0) -------------------*/

struct VS_SSAO_IN
{
    float3 vPosition : POSITION; /* 들어온값 */ 
    float2 ToFarPlaneIndex : NORMAL;
    float3 vTexcoord : TEXCOORD;
};

struct VS_SSAO_OUT
{
    float4 vPosition : SV_POSITION;
    float3 ToFarPlane : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};


VS_SSAO_OUT VS_SSAO(VS_SSAO_IN In)
{
    VS_SSAO_OUT Out = (VS_SSAO_OUT) 0;
    
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.0f), matWVP);
    
    Out.ToFarPlane = FrustumCorner[In.ToFarPlaneIndex.x].xyz;
    
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

/* ------------------- Pixel Shader (0) -------------------*/

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 ToFarPlane : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

// 교합값을 찾는 기능
float OcclusionFunction(float distZ)
{
    float gSurfaceEpsilon = 0.05f;
    float gOcclusionFadeStart = 0.2f;
    float gOcclusionFadeEnd = 2.0f;
    float occlusion = 0.0f;
    
    if (distZ > gSurfaceEpsilon)
    {
        float fadeLength = gOcclusionFadeEnd - gOcclusionFadeStart;
        
        // DistZ가 증가함에 따라 폐색의 값이 1에서 0으로 선형적으로 감소한다.
        occlusion = saturate((gOcclusionFadeEnd - distZ) / fadeLength);
    }
    
    return occlusion;
}

PS_OUT PS_SSAO(PS_IN In) : SV_Target
{
    PS_OUT Out = (PS_OUT) 0;
    
	// p -- the point we are computing the ambient occlusion for.
	// n -- normal vector at p.
	// q -- a random offset from p.
	// r -- a potential occluder that might occlude p.
    
    float gOcclusionRadius = 0.5f;
    
    float4 normalDepth = NormalDepthMap.Sample(SSAONormalDepth, In.vTexcoord, 0.0f);
    
    // 카메라 공간의 픽셀좌표와 Z 값
    float3 n = normalDepth.xyz;
    float pz = normalDepth.w;
    
    // p = t * In.ToFarPlane.
	// p.z = t * In.ToFarPlane.z
	// t = p.z / pin.ToFarPlane.z
    
    float3 p = (pz / In.ToFarPlane.z) * In.ToFarPlane;
    
    // 랜덤 벡터 맵의 값을 구하고 값을 [0, 1]에서 [-1, 1] 범위로 변경한다.
    // 랜덤 벡터 타일링에 주의.
    float3 randVec = 2.0f * RandomVecMap.Sample(SSAORandVec, 4.0f * In.vTexcoord, 0.0f).rgb - 1.0f;
    
    // 폐색 값의 합
    float occlusionSum = 0.0f;
    
    // 샘플 수 설정
    const int sampleCount = 14;
   
    // n 벡터 방향으로 점 p의 반구 범위에 있는 인접 점을 샘플링하는 것은 일반적으로 8/16/32로 나누어진다.
    // 여기서는 14개의 변위 벡터, 즉 14개의 샘플링 점을 갖게 된다.
    [unroll]
    for (int i = 0; i < sampleCount; ++i)
    {
        // 변위 벡터는 고정되고 균일하게 분산된다
        // 왜냐하면 변위가 같은 방향에서 동일하기 때문에 변위 벡터를 사용하여 반영하면 무작위로 동일한 변위 분산 벡터를 얻는다.
        // 무작위 벡터를 사용하여 압사하고 고정한다.
        // 큐브 벡터는 법선 벡터로 사용되어 반사 벡터를 최종 변위 벡터로 찾는다.
        float3 offset = reflect(g_OffsetVector[i].xyz, randVec);
        
        // 변위 벡터가 표면 뒤에 있는 경우 Flip은 점 q가 항상 시점에 가까운 p측면에 있도록 변위 벡터를 뒤집는다.
        // sign()은 값의 부호를 찾는다.
        float flip = sign(dot(offset, n));
        
        // 점 p의 교합반경에서 점 q를 찾는다.
        float3 q = p + flip * gOcclusionRadius * offset;
        
        // q를 투영하고 투영된 텍스쳐 좌표를 생성.
        // q 자체가 카메라 공간에 있으므로 투영된 텍스쳐 좌표로 변경된다.
        // 관점 분할은 마지막에 수행되어야 한다.
        float4 ProjQ = mul(float4(q, 1.0f), ViewToTexSpcace);
        ProjQ /= ProjQ.w;

        // 원점에서 q까지의 카메라 공간에서 NormalDepthMap에 해당하는 최소 깊이 값을 찾는다.
        // 이 최소 깊이는 카메라 공간의 q 깊이와 같지 않다.
        float rz = NormalDepthMap.Sample(SSAONormalDepth, ProjQ.xy, 0.0f).a;
        
        // 다시 유사 삼각형 원리를 사용한다.
        float3 r = (rz / q.z) * q;
        
        float distZ = p.z - r.z;
        float dp = max(dot(n, normalize(r - p)), 0.0f);
        float occlusion = dp * OcclusionFunction(distZ);
        
        occlusionSum += occlusion;
    }
    
    occlusionSum /= (float) sampleCount;
    
    float access = 1.0f - occlusionSum;
    
    // 너무 어둡다는 피드백이 있어서 0.5~1 사이의 값으로 사상함
    Out.vColor = (saturate(pow(access, 4.0f)) + 1) * 0.4;
    
    return Out;
}

/* ------------------- Vertex Shader(1) -------------------*/

//struct VS_GOD_IN
//{
//    float3 vPosition : POSITION; /* 들어온값 */
//    float2 vTexcoord : TEXCOORD0;
//    float3 vFarPlaneIndex : NORMAL;
//};

//struct VS_GOD_OUT
//{
//    float4 vPosition : SV_POSITION;
//    float3 vTexcoord : TEXCOORD0;
//    float2 vTexcoord1 : TEXCOORD1;
//};

//VS_GOD_OUT VS_GODRAY(VS_GOD_IN In)
//{
//    VS_GOD_OUT Out = (VS_GOD_OUT) 0;
    
//    return Out;
//}

///* ------------------- Pixel Shader (1) -------------------*/
//struct PS_GOD_IN
//{
    
//};

//struct PS_GOD_Out
//{
    
//};

//PS_GOD_Out PS_GODRAY(PS_GOD_IN In)
//{
//    PS_GOD_Out Out = (PS_GOD_Out) 0;

//    return Out;
//}

/* ------------------- Technique -------------------*/ 

technique11 DefaultTechnique
{
    pass SSAO // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_SSAO();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SSAO();
    }
   // pass GodRay // 0
   // {
   //     /* 옵션조절 아직안함 */ 
   //     SetRasterizerState(RS_Default);
   //     SetDepthStencilState(DSS_None, 0);
   //     SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 1.f), 0xffffffff);
   //     VertexShader = compile vs_5_0 VS_GODRAY();
   //     GeometryShader = NULL;
   //     HullShader = NULL;
   //     DomainShader = NULL;
   //     PixelShader = compile ps_5_0 PS_GODRAY();
   // }

}
