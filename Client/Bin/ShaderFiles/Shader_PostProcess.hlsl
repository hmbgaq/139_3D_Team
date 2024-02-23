include "Shader_Defines.hlsli"
#define NUM_SAMPLERS 64

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* ssao */
vector  g_OffsetVector[14];
vector  FrustumCorner[4];
matrix  ViewToTexSpcace;

Texture2D g_NormalDepthTarget;
Texture2D g_RandomVectorTexture;

/* GodRay */ 

/* ------------------- Vertex Shader(0) -------------------*/

struct VS_SSAO_IN
{
    float3 vPosition : POSITION; /* 들어온값 - 예제에서는 여기가 Proj * Texture임 */ 
    float3 ToFarPlaneIndex : NORMAL;
    float2 vTexcoord : TEXCOORD;
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
    // depth(q)
	//
	//       1.0     -------------\
	//               |           |  \
	//               |           |    \
	//               |           |      \ 
	//               |           |        \
	//               |           |          \
	//               |           |            \
	//  ------|------|-----------|-------------|---------|--> zv
	//        0     Eps          z0            z1        
	//
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

PS_OUT PS_SSAO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
	// p -- the point we are computing the ambient occlusion for.
	// n -- normal vector at p.
	// q -- a random offset from p.
	// r -- a potential occluder that might occlude p.
    
    float gOcclusionRadius = 0.5f;
    
	// Get viewspace normal and z-coord of this pixel.  The tex-coords for
	// the fullscreen quad we drew are already in uv-space.
    float4 vNormalDepth = g_NormalDepthTarget.Sample(SSAONormalDepth, In.vTexcoord, 0.0f);
    
    float3 vNormalV = vNormalDepth.xyz;
    float fViewZ = vNormalDepth.w * 1000.f;
    // p = t * In.ToFarPlane.
	// p.z = t * In.ToFarPlane.z
	// t = p.z / pin.ToFarPlane.z
    
    float3 vPosV = (fViewZ / In.ToFarPlane.z) * In.ToFarPlane;
    
    // 랜덤 벡터 맵의 값을 구하고 값을 [0, 1]에서 [-1, 1] 범위로 변경한다.
    // 랜덤 벡터 타일링에 주의.
    float3 randVec = 2.0f * g_RandomVectorTexture.Sample(SSAORandVec, 4.0f * In.vTexcoord, 0.0f).rgb - 1.0f;
    
    // 폐색 값의 합
    float fOcclusionSum = 0.0f;
    
    // 샘플 수 설정
    const int sampleCount = 14;
   
    // n 벡터 방향으로 점 p의 반구 범위에 있는 인접 점을 샘플링하는 것은 일반적으로 8/16/32로 나누어진다.
    // 여기서는 14개의 변위 벡터, 즉 14개의 샘플링 점을 갖게 된다.
    [unroll]
    for (int i = 0; i < sampleCount; ++i)
    {
		// Are offset vectors are fixed and uniformly distributed (so that our offset vectors
		// do not clump in the same direction).  If we reflect them about a random vector
		// then we get a random uniform distribution of offset vectors.
        float3 offset = reflect(g_OffsetVector[i].xyz, randVec);
        
        float flip = sign(dot(offset, vNormalV.xyz));
		// Sample a point near p within the occlusion radius.
        //float3 q = p + gOcclusionRadius * offset;
        
        // 점 p의 교합반경에서 점 q를 찾는다.
        float3 q = vPosV + flip * gOcclusionRadius * offset; // q = 차폐 검사할 벡터
        
		// Project q and generate projective tex-coords.
        float4 ProjQ = mul(float4(q, 1.0f), ViewToTexSpcace);
        ProjQ /= ProjQ.w;
        
		// Find the nearest depth value along the ray from the eye to q (this is not
		// the depth of q, as q is just an arbitrary point near p and might
		// occupy empty space).  To find the nearest depth we look it up in the depthmap.
        float fRealZ = g_NormalDepthTarget.Sample(SSAONormalDepth, ProjQ.xy, 0.f).w;
        fRealZ *= 1000.f;
        
        // 다시 유사 삼각형 원리를 사용한다.
        float3 r = (fRealZ / q.z) * q;
        
        float distZ = vPosV.z - r.z;
        float dp = max(dot(vNormalV, normalize(r - vPosV)), 0.0f);
        float fOcclusion = dp * OcclusionFunction(distZ);
		
        fOcclusionSum += fOcclusion;
    }
    
    fOcclusionSum /= (float) sampleCount;
    
    float access = 1.0f - fOcclusionSum;
    
    // 너무 어둡다는 피드백이 있어서 0.5~1 사이의 값으로 사상함
    Out.vColor = (saturate(pow(access, 4.0f)) + 1) * 0.4;
    
    return Out;
}

/* ------------------- Vertex Shader(1) -------------------*/

struct VS_GOD_IN
{
    float3 vPosition : POSITION; /* 들어온값 */
    float2 vTexcoord : TEXCOORD0;
    float3 vFarPlaneIndex : NORMAL;
};

struct VS_GOD_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
    float2 vTexcoord1 : TEXCOORD1;
};

VS_GOD_OUT VS_GODRAY(VS_GOD_IN In)
{
    VS_GOD_OUT Out = (VS_GOD_OUT) 0;
    
    return Out;
}

///* ------------------- Pixel Shader (1) -------------------*/
struct PS_GOD_IN
{
    
};

struct PS_GOD_Out
{
    
};

PS_GOD_Out PS_GODRAY(PS_GOD_IN In)
{
    PS_GOD_Out Out = (PS_GOD_Out) 0;
    
    //float2 vTexcoord = In.vTexcoord;
    
    //float4 lightData = vLightShaftValue;
    // // x = Density, y = Weight, z = Decay , w = Exposure
     
    // // MaskColor
    //float4 vMaskColor = SunOccluderTexture.SampleLevel(LinearSamplerClamp, vTexcoord, 0);
    //float4 vTempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //if (vMaskColor.a == 1.0f)
    //    vTempColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    //float2 lightPosition = vScreenSunPosition;
    //float2 deltaTexcoord = (vTexcoord - lightPosition);

    
    //deltaTexcoord *= lightData.x / NUM_SAMPLERS; // Density / NUM_SAMPLERS
    
    //float illuminationDecay = 1.0f;
    //float3 accumulatedGodRays = float3(0.0f, 0.0f, 0.0f);
    //float3 accumulated = 0.0f;
    
    //for (int i = 0; i < NUM_SAMPLERS; ++i)
    //{
    //    vTexcoord.xy -= deltaTexcoord;
    //    float4 sample = SunOccluderTexture.SampleLevel(LinearSamplerClamp, vTexcoord.xy, 0);
    
    //    float4 vSrcColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //    if (sample.a == 1.0f)
    //        vSrcColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    //    vSrcColor.rgb *= illuminationDecay * lightData.y; // illuminationDecay * Weight
    //    accumulated += vSrcColor.rgb;
    //    illuminationDecay *= lightData.z; // illuminationDecay *= lightData.z (Decay)
    //}
    
    //accumulated *= lightData.w;
    //Out.vColor = saturate(float4(vTempColor.rgb + accumulated, 1.0f));
    
    return Out;
}

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

   // pass GodRay // 1
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
