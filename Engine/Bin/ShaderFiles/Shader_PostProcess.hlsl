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
    float2 vTexcoord : TEXCOORD0;
    float3 vFarPlaneIndex : NORMAL;
};

struct VS_SSAO_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vFarPlane : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};


VS_SSAO_OUT VS_SSAO(VS_SSAO_IN In)
{
    VS_SSAO_OUT Out = (VS_SSAO_OUT) 0;
    
	/* In.vPosition * 월드 * 뷰 * 투영 */
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.0f), matWVP);
    
    // 원점에서 각 원거리 클리핑 평면의 각 픽셀까지의 벡터를 얻기 위한 보간을 용이하게 하기 위해
    // 각 카메라 공간에서 원점부터 원거리 클리핑 평면의 모서리까지의 벡터를 찾는다.
    Out.vFarPlane = FrustumCorner[In.vFarPlaneIndex.x].xyz;
    
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

/* ------------------- Pixel Shader (0) -------------------*/

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vFarPlane : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

// 교합값을 찾는 기능
float OcclusionFunction(float distZ)
{
    float occlusion = 0.0f;
    
    // 너무 가까우면 두 점이 같은 평면에 있으면 폐색을 일으킬 수 없는 것으로 간주한다.
    if (distZ > 0.05f)
    {
        float fadeLength = 2.f - 0.2f;
        
        // DistZ가 증가함에 따라 폐색의 값이 1에서 0으로 선형적으로 감소한다.
        occlusion = saturate((2.f - distZ) / fadeLength);
    }
    
    return occlusion;
}

PS_OUT PS_SSAO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // p : 점 p의 AmbientOcclusion 값을 계산한다.
    // n : 점 p에서 법선 벡터
    // q : 점 p로부터의 무작위 벡터
    // r : p를 모호하게 할 수 있는 잠재적인 점
    
    // 카메라 공간의 픽셀의 법선 벡터와 Z 값을 가지고온다.
    // ScreenQuad의 좌표는 이미 Texture 좌표공간에 있다.
    float4 normalDepth = NormalDepthMap.Sample(SSAONormalDepth, In.vTexcoord, 0.0f);
    
    // 카메라 공간의 픽셀좌표와 Z 값
    float3 n = normalDepth.xyz;
    float pz = normalDepth.w;
    
    // 유사 삼각형의 원리를 사용, 카메라 공간에서 픽셀의 위치를 재구성한다.
    // 해당 원점에서 먼 클리핑 평면까지 선분은 점 P를 통과한다.
    // P = t * input.ToFarPlane이 된다.
    // 따라서 pz = t * input.ToFarPlane.z가 된다.
    // pz = t * input.ToFarPlane.z가 있고
    // t = pz / input.ToFarPlane.z가 있다.
    // P = (Pz/input.ToFarPlane), z) * input.ToFarPlane
    
    float3 p = (pz / In.vFarPlane.z) * In.vFarPlane;
    
    // 랜덤 벡터 맵의 값을 구하고 값을 [0, 1]에서 [-1, 1] 범위로 변경한다.
    // 랜덤 벡터 타일링에 주의.
    float3 randVec = 2.0f * RandomVecMap.Sample(SSAORandVec, 4.0f * In.vTexcoord, 0.0f).rgb - 1.0f;
    
    // 폐색 값의 합
    float occlusionSum = 0.0f;
    
    // 샘플 수 설정
    const int sampleCount = 26;
   
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
        float3 q = p + flip * 0.5f * offset;
        
        // q를 투영하고 투영된 텍스쳐 좌표를 생성.
        // q 자체가 카메라 공간에 있으므로 투영된 텍스쳐 좌표로 변경된다.
        // 관점 분할은 마지막에 수행되어야 한다.
        float4 ProjQ = mul(float4(q, 1.0f), ViewToTexSpcace);
        ProjQ /= ProjQ.w;

        // 원점에서 q까지의 카메라 공간에서 NormalDepthMap에 해당하는 최소 깊이 값을 찾는다.
        // 이 최소 깊이는 카메라 공간의 q 깊이와 같지 않다.
        float rz = NormalDepthMap.Sample(SSAONormalDepth, ProjQ.xy, 0.0f).w;
        
        // 다시 유사 삼각형 원리를 사용한다.
        float3 r = (rz / q.z) * q;

        // dot(n, normalize(rp))는 점 p 앞의 점 r의 폐색 값을 계산한다. 각도가 작을 수록 폐색 값이 커지고,
        // r과 p가 Z거리에서 가까울수록 폐색이 커진다.(임계값에 가까움)
        // 셀프 섀도잉을 방지하기 위해 폐색 값은 0이다.
        float distZ = p.z - r.z;
        float dp = max(dot(n, normalize(r - p)), 0.0f);
        float occlusion = dp * OcclusionFunction(distZ);
        
        occlusionSum += occlusion;
    }
    
    // 최종 폐색값 노멀라이즈
    occlusionSum /= (float) sampleCount;
    
    // AmbientLight 허용 값 찾기
    float access = 1.0f - occlusionSum;
    
    // SSAOMap의 효과를 선명하게 할려면 pow를 사용
    // SSAOMap은 회색조이다.
    Out.vColor =  saturate(pow(access, 4.0f));
    
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
