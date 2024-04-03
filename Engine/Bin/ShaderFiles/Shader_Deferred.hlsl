//#include "Shader_PBR.hlsl"
#include "Shader_MyPBR.hlsl"
#include "DitherUtil.hlsli"
//#include "Shader_Defines.hlsli"
//#include "HeightFogUsage.hlsl"
//#pragma multi_compile _ HF_FOG_ENABLED HF_LIGHT_ATTEN

/*=============================================================
 
                             Struct 
                                
==============================================================*/
struct FOG_DESC
{
    bool    bFog_Active;
    float   fFogStartDepth;
    float   fFogStartDistance;
    float   fFogDistanceValue;
    float   fFogHeightValue;
    float   fFogDistanceDensity;
    float   fFogHeightDensity;
    float   padding;              // 4 
    float4  vFogColor;
};

struct BLOOMRIM_DESC
{
    // 패딩을 추가하지않아도 메모리가 정렬됨 
    bool    bBloomBlur_Active;
    bool    bRimBlur_Active;
};
/*=============================================================
 
                             Variable 
                                
==============================================================*/
// Directional
vector g_vLightDir;

// Spot
vector g_vLightPos;
float  g_fLightRange;

// Point - g_vLightDir, g_vLightPos 둘도 사용
float g_fRange;
float g_fCutOff;
float g_fOuterCutOff;
float g_fVolumetricStrength;

// Common 
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;
float  g_fLightIntensity;

/* ========== Base ==========*/
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ProjMatrixInv, g_ViewMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;
matrix g_CamProjMatrix, g_CamViewMatrix; /* ssr에서 사용 */
vector g_vCamPosition;
float  g_CamFar;
float  g_LightFar;

vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);


/* ========== Texture2D ==========*/
Texture2D g_PriorityTarget;         /* Skybox - Priority */
Texture2D g_DiffuseTexture;         /* Out.Diffuse */ 
Texture2D g_NormalTexture;          /* Out.Normal */
Texture2D g_DepthTarget;            /* Out.Depth 카메라가 본 깊이 */ 
Texture2D g_ORMTexture;             /* Out.ORM */ 
Texture2D g_ShadeTexture;           /* LightAcc - 빛이본 깊이 */ 
Texture2D g_EmissiveTarget;         /* Emissive - For. PBR */
Texture2D g_SpecularTexture;        /* LightAcc - 빛이 본 Spec */
Texture2D g_ShadowDepthTexture;     /* Render_Shadow 결과 */
Texture2D g_SSAOTexture;            /* Pre - HBAO+ */
Texture2D g_BloomRimTarget;         /* BloomRim Blur */ 
Texture2D g_PerlinNoiseTexture;     /* Shor Fog - Noise Texture */
Texture2D g_Effect_DiffuseTarget;   /* Effect - Diffuse */
Texture2D g_OutlineTarget;          /* RenderGroup - Outline */
Texture2D g_VoxelReadTexture;

/* ========== GameObject ==========*/
BLOOMRIM_DESC g_Bloom_Rim_Desc;

/* ========== Fog ==========*/
float2 g_vFogUVAcc = { 0.f, 0.f };
FOG_DESC g_Fogdesc;

/* ========== Deferred Active ==========*/
bool g_bSSAO_Active;
bool g_bFog_Active;
bool g_bShadow_Active;

/* ========== PBR ==========*/
float g_fBias;
float3 g_vLightColor;

/* ========== Cascade Shadow ============= */
float g_fStaticBias;
matrix g_CascadeProj[3];

Texture2D g_CascadeTarget1;
Texture2D g_CascadeTarget2;
Texture2D g_CascadeTarget3;


/*=============================================================
 
                             IN/OUT  
                                
==============================================================*/
struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

/*=============================================================
 
                             Function 
                                
==============================================================*/

float3 Compute_HeightFogColor(float3 vOriginColor, float3 toEye, float fNoise, FOG_DESC desc)
{
    /*  vOriginColor : 안개없이 원래 그리는 색상 
        toEye : 카메라에서 픽셀 바라보는벡터 */ 
    
    // 지정 범위 Distance
    float pixelDistance = desc.fFogDistanceDensity * (length(g_vCamPosition.w - toEye) - desc.fFogStartDepth);
    
	// 지정 범위 Height
    float pixelHeight = desc.fFogHeightDensity * toEye.y;
    
    float distanceOffset = min(pow(2.0f, pixelDistance - desc.fFogStartDistance), 1.0f);
    float heightOffset = min(pow(1.2f, -(pixelHeight + 3.0f)), 1.0f);
    
	// 거리 기반 안개 강도 설정
    float distanceValue = exp(0.01f * pow(pixelDistance - desc.fFogDistanceValue, 3.0f));
    float fogDistanceFactor = min(distanceValue, 1.0f);

	// 높이 기반 안개 강도 설정
    float heightValue = (pixelHeight * desc.fFogHeightValue) - 0.1f;
    float fogHeightFactor = pow(pow(2.0f, -heightValue), heightValue) * (1.0f - distanceOffset);

	// 두 요소를 결합한 최종 요소
    float fogFinalFactor = min(fogDistanceFactor * fogHeightFactor * fNoise, 1.0f) + min(distanceOffset * heightOffset, 1.0f) + 0.01f;

    vector vFogColor = desc.vFogColor;
    
	// 최종 혼합 색상
    return lerp(vOriginColor.rgb, vFogColor.xyz, fogFinalFactor);
}

// https://github.com/Unity-Technologies/VolumetricLighting/blob/master/Assets/VolumetricFog/Shaders/Scatter.compute
float4 Accumulate(int z, float4 result, float4 colorDensityPerSlice, float VolumeSize)
{
    colorDensityPerSlice.a = max(colorDensityPerSlice.a, 0.000001);
    //float thickness = GetSliceThickness(z, CameraNearFar_FrameIndex_PreviousFrameBlend.x, CameraNearFar_FrameIndex_PreviousFrameBlend.y);
    //float sliceTransmittance = exp(-colorDensityPerSlice.a * thickness * ThicknessFactor);

    // Seb Hillaire's improved transmission by calculating an integral over slice depth instead of
	// constant per slice value. Light still constant per slice, but that's acceptable. See slide 28 of
	// Physically-based & Unified Volumetric Rendering in Frostbite
	// http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/
    float sliceTransmittance = exp(-colorDensityPerSlice.a / VolumeSize);

    float3 sliceScattering = colorDensityPerSlice.rgb * (1.0f - sliceTransmittance) / colorDensityPerSlice.a;

    result.rgb += sliceScattering * result.a;
    result.a *= sliceTransmittance;
    return result;
}

static float3 GetViewSpacePosition(float2 texcoord, float depth)
{
    float4 clipSpaceLocation;
    clipSpaceLocation.xy = texcoord * 2.0f - 1.0f;
    clipSpaceLocation.y *= -1;
    clipSpaceLocation.z = depth;
    clipSpaceLocation.w = 1.0f;
    float4 homogenousLocation = mul(clipSpaceLocation, g_ProjMatrixInv);
    return homogenousLocation.xyz / homogenousLocation.w;
}

float DoAttenuation(float distance, float range)
{
    float att = saturate(1.0f - (distance * distance / (range * range)));
    return att * att;
}

float4 DoDiffuse(float4 lightColor, float3 L, float3 N)
{
    float NdotL = max(0, dot(N, L));
    return lightColor * NdotL;
}

float4 DoSpecular(float4 lightColor, float shininess, float3 L, float3 N, float3 V)
{
    float3 R = normalize(reflect(-L, N));
    float RdotV = max(0.0001, dot(R, V));
    return lightColor * pow(RdotV, shininess);
}


/*=============================================================
 
                         Vertex Shader 
                                
==============================================================*/
/* ----------------------------------------------- */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;

}

inline bool IsSaturated(float value)
{
    return value == saturate(value);
}
inline bool IsSaturated(float2 value)
{
    return IsSaturated(value.x) && IsSaturated(value.y);
}
inline bool IsSaturated(float3 value)
{
    return IsSaturated(value.x) && IsSaturated(value.y) && IsSaturated(value.z);
}
inline bool IsSaturated(float4 value)
{
    return IsSaturated(value.x) && IsSaturated(value.y) && IsSaturated(value.z) && IsSaturated(value.w);
}

/*=============================================================
 
                          Pixel Shader  
                                
==============================================================*/
/* ------------------ 0 - DEBUG ------------------ */

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

/* ------------------ 1 - Directional ------------------ */
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    // g_vLightDir, g_vLightDiffuse, g_vLightAmbient, g_vLightSpecular 바인딩 
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
	//vector		
	
	/* 0, 1 -> -1, 1 */
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    Out.vShade = g_vLightDiffuse * min((max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)), 1.f);
    
    float fViewZ = vDepthDesc.y * g_CamFar;

    vector vWorldPos;

	/* 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬* 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLook = vWorldPos - g_vCamPosition;
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

    return Out;
}

/* ------------------ 2 - Point ------------------ */

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    //g_vLightPos, g_fLightRange, g_vLightDiffuse, g_vLightAmbient, g_vLightSpecular 바인딩 
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);

	/* 0, 1 -> -1, 1 */
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_CamFar;

    vector vWorldPos;

	/* 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬* 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


    float4 vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);

    float fAtt = max((g_fLightRange - fDistance) / g_fLightRange, 0.f);

    Out.vShade = g_vLightDiffuse * min((max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)), 1.f) * fAtt;

	
    vector vLook = vWorldPos - g_vCamPosition;
    vector vReflect = reflect(normalize(vLightDir), vNormal);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f) * fAtt;

    return Out;
}


/* ------------------ 3 - Spot  ------------------ */
PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 1;

    float depth = max(In.vPosition.z, g_DepthTarget.SampleLevel(ClampSampler,  In.vTexcoord, 2));
    float3 viewSpacePosition = GetViewSpacePosition(In.vTexcoord, depth);
    float3 V = float3(0.0f, 0.0f, 0.0f) - viewSpacePosition;
    float cameraDistance = length(V);
    V /= cameraDistance;
    
    const uint SAMPLE_COUNT = 16;
    float3 rayEnd = float3(0.0f, 0.0f, 0.0f);
    float stepSize = length(viewSpacePosition - rayEnd) / SAMPLE_COUNT;
    viewSpacePosition = viewSpacePosition + V * stepSize * BayerDither(In.vPosition.xy);
    
    float marchedDistance = 0;
    float accumulation = 0;
    
	[loop(SAMPLE_COUNT)]
    for (uint i = 0; i < SAMPLE_COUNT; ++i)
    {
        float3 L = g_vLightPos.xyz - viewSpacePosition;
        float distSquared = dot(L, L);
        float dist = sqrt(distSquared);
        L /= dist;
    
        float spotFactor = dot(L, normalize(-g_vLightDir.xyz));
        float spotCutOff = g_fOuterCutOff;
    
		[branch]
        if (spotFactor > spotCutOff)
        {
            float attenuation = DoAttenuation(dist, g_fRange);
            float conAtt = saturate((spotFactor - g_fOuterCutOff) / (g_fCutOff - g_fOuterCutOff));
            conAtt *= conAtt;
            attenuation *= conAtt;
            
			//[branch]
            //if (lightData.castsShadows)
            //{
            //    float4 shadowMapCoords = mul(float4(viewSpacePosition, 1.0), shadowData.shadowMatrices[0]);
            //    float3 UVD = shadowMapCoords.xyz / shadowMapCoords.w;
            //
            //    UVD.xy = 0.5 * UVD.xy + 0.5;
            //    UVD.y = 1.0 - UVD.y;
            //    [branch]
            //    if (IsSaturated(UVD.xy))
            //    {
            //        float shadowFactor = CalcShadowFactor_PCF3x3(ShadowSampler, ShadowMap, UVD, shadowData.shadowMapSize, shadowData.softness);
            //        attenuation *= shadowFactor;
            //    }
            //}
            accumulation += attenuation;
        }
        marchedDistance += stepSize;
        viewSpacePosition = viewSpacePosition + V * stepSize;
    }
    
    accumulation /= SAMPLE_COUNT;
    Out.vSpecular = max(0, float4(accumulation * g_vLightDiffuse.rgb * g_fVolumetricStrength, 1));
    
    vector  vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector  vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
    float4  vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    float   fViewZ = vDepthDesc.y * g_CamFar;
    vector  vWorldPos;

	/* 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬* 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    float4 vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    float fAtt = max((g_fLightRange - fDistance) / g_fLightRange, 0.f);

    Out.vShade = g_vLightDiffuse * min((max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)), 1.f) * fAtt;

    return Out;
}

/* ------------------ 4 - Deferred ------------------ */

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDiffuse.a == 0.f)
    {
        float4 vPriority = g_PriorityTarget.Sample(LinearSampler, In.vTexcoord);
        if (vPriority.a == 0.f)
            discard;
        
        Out.vColor = vPriority;
    }
    else
    { 
        // MRT_LightAcc : Shade 
        vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
        vShade = saturate(vShade);
	
        // MRT_GameObject : Specular 
        vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
        vSpecular = saturate(vSpecular);
	
    
        // Target_HBAO
        vector vSSAO = float4(1.f, 1.f, 1.f, 1.f);
        if (g_bSSAO_Active)
            vSSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexcoord);
    
        Out.vColor = (vDiffuse * vShade * vSSAO) + vSpecular;
    }
    // MRT_GameObject : Depth
    vector vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
    
    float fViewZ = vDepthDesc.y * g_CamFar;
	
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
        
    if (true == g_bFog_Active)
    {
        float3 vTexCoord = float3((vWorldPos.xyz * 100.f) % 12800.f) / 12800.f;
        vTexCoord.x += g_vFogUVAcc.x;
        vTexCoord.y += g_vFogUVAcc.y;
    
        float fNoise = g_PerlinNoiseTexture.Sample(LinearSampler, vTexCoord.xy).r;
    
        float3 vFinalColor = Compute_HeightFogColor(Out.vColor.xyz, (vWorldPos - g_vCamPosition).xyz, fNoise, g_Fogdesc);
        
        //float4 colorDensityPerSlice = g_VoxelReadTexture.Sample(ClampSampler, In.vTexcoord);
        //
        //if (colorDensityPerSlice.a == 0)
        //    discard;
        //
        //float4 accumulateResult = Accumulate(0, float4(0.0f, 0.0f, 0.0f, 1.0f), colorDensityPerSlice, g_Fogdesc.VloumeSize_Z);
        //
        //vFinalColor *= accumulateResult.rgb;
        
        Out.vColor = vector(vFinalColor.rgb, 1.f);
    }
    
    if (true == g_bShadow_Active)
    {
        vWorldPos = mul(vWorldPos, g_LightViewMatrix);
        vWorldPos = mul(vWorldPos, g_LightProjMatrix);
   
        float2 vUV = (float2) 0.0f;
   
        vUV.x = (vWorldPos.x / vWorldPos.w) * 0.5f + 0.5f;
        vUV.y = (vWorldPos.y / vWorldPos.w) * -0.5f + 0.5f;
   
        float4 vLightDepth = g_ShadowDepthTexture.Sample(LinearSampler, vUV);
   
        if (vWorldPos.w - 0.1f > vLightDepth.x * g_LightFar) /* LightFar */ 
            Out.vColor = Out.vColor * 0.8f;
    }
    
    //Out.vColor += vEffect;
    Out.vColor.a = 1.f;
    
    return Out;
}

/* ------------------ 5 - PBR Deferred ------------------ */

PS_OUT PS_MAIN_PBR_DEFERRED(PS_IN In)
{
    // 주의사항 : ORM  텍스쳐 넣어야함 + hbao+ 수행해야함 
    // 추가사항 : g_vLightDir, g_fBias, g_vLightDir
    PS_OUT Out = (PS_OUT) 0;
	
    // 감마보정 -> Linear space로 변형후 빛계산을 하고 다시 감마보정을 통해서 그린다.
    float gamma = 2.2f;
    // Diffuse -> Albedo, Properties-> Specular, ORM : Occulusion, Roughness Metallic, 
    vector vAlbedo = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vAlbedo.a == 0.f)
    {
        float4 vPriority = g_PriorityTarget.Sample(LinearSampler, In.vTexcoord);
        if (vPriority.a == 0.f)
            discard;
        
        Out.vColor = vPriority;
        
        return Out;
    }
    vAlbedo = pow(vAlbedo, gamma);
    
    vector vNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 N = vNormal.xyz * 2.f - 1.f;
    vector vORMDesc = g_ORMTexture.Sample(LinearSampler, In.vTexcoord);
    /* (R)Occlusion ,(G)Roughness , (B)Metalic */
    
    //float fAmbient_Occlusion = vORMDesc.r;
    float fRoughness = vORMDesc.g;
    float fMetallic = vORMDesc.b;
    
    vector vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
    
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    vSpecular = saturate(vSpecular);
    
    float fAO = 1.f; 
    fAO = g_SSAOTexture.Sample(LinearSampler, In.vTexcoord).r; // Ambient Occulusion은 HBAO+로 
    
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vNormal.w;
    vWorldPos.w = 1.f;
    
    float fViewZ = vDepthDesc.w * g_CamFar;
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    // = Shadow ====================
       
    float ShadowColor = 1.f;
    
    if (true == g_bShadow_Active)
    {
        vector vPosition = mul(vWorldPos, g_LightViewMatrix);
        vPosition = mul(vPosition, g_LightProjMatrix);
   
        float2 vUV = (float2) 0.0f;
   
        vUV.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
        vUV.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
   
        float4 vLightDepth = g_ShadowDepthTexture.Sample(LinearSampler, vUV);
   
        if (vPosition.w - 0.1f > vLightDepth.x * g_LightFar) /* LightFar */ 
            ShadowColor = 0.8f;
    }
    
    // =====================
    float3 V = normalize(g_vCamPosition.xyz - vWorldPos.xyz);
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, vAlbedo.xyz, fMetallic); // 반사율 F0

	// calculate per-light radiance
    float3 L = normalize(-g_vLightDir);
    float3 H = normalize(V + L);

    float3 vBRDF = { 0.f, 0.f, 0.f };
    // BRDF를 통해 물체의 표면 속성을 모델링하고 이를 통해 자연스러운 조명과 반사를 구현한다. 
    //vBRDF = New_BRDF(fRoughness, fMetallic, vAlbedo.xyz, F0, N, V, L, H, fAO); 
   
    float3 vEmissive = g_EmissiveTarget.Sample(LinearSampler, In.vTexcoord).rgb;
    vEmissive = pow(vEmissive, gamma);
    
    Out.vColor.rgb = vBRDF * ShadowColor + vEmissive + vSpecular;
    
    Out.vColor.a = 1.f;
	
    return Out;
}

/* ------------------ 6 - PBR Deferred ------------------ */
PS_OUT PS_MAIN_NEW_PBR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 감마보정 -> Linear space로 변형후 빛계산을 하고 다시 감마보정을 통해서 그린다.
    float gamma = 2.2f;
    
    vector vAlbedo = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vAlbedo = pow(vAlbedo, gamma);
    if (vAlbedo.a == 0.f)
    {
        float4 vPriority = g_PriorityTarget.Sample(LinearSampler, In.vTexcoord);
        if (vPriority.a == 0.f)
            discard;
        
        Out.vColor = vPriority ;
        
        return Out;
    }
    
    // ORM : (R)Occlusion ,(G)Roughness , (B)Metalic */ 에 저장 
    vector vORMDesc = g_ORMTexture.Sample(LinearSampler, In.vTexcoord);
    float fAmbient_Occlusion = vORMDesc.r;
    float fRoughness = vORMDesc.g;
    float fMetallic = vORMDesc.b;
    float fAO;
    fAO = g_SSAOTexture.Sample(LinearSampler, In.vTexcoord).r; // Ambient Occulusion은 HBAO+로 

    // N
    vector vNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 N = vNormal.xyz * 2.f - 1.f;

    // F0
    float3 F0 = 0.04f; //  F0 : reflectance ratio - 0.04 looks visually correct for non-metallic sufaces
    F0 = lerp(F0, vAlbedo.rgb, fMetallic); // 슈레넬에서 쓸 R0 반사율 

    // N(위에), V, L, H
    vector vDepthDesc = g_DepthTarget.Sample(LinearSampler, In.vTexcoord);
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vNormal.w;
    vWorldPos.w = 1.f;

    float fViewZ = vDepthDesc.w * g_CamFar;
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    float3 V = normalize(g_vCamPosition.xyz - vWorldPos.xyz);
    float3 L = normalize(-g_vLightDir);
    float3 H = normalize(V + L);
        
    float ShadowColor = 1.f;
    if (true == g_bShadow_Active)
    {
        vector vPosition = mul(vWorldPos, g_LightViewMatrix);
        vPosition = mul(vPosition, g_LightProjMatrix);
   
        float2 vUV = (float2) 0.0f;
   
        vUV.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
        vUV.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
   
        float4 vLightDepth = g_ShadowDepthTexture.Sample(LinearSampler, vUV);
   
        if (vPosition.w - 0.1f > vLightDepth.x * g_LightFar) /* LightFar */ 
            ShadowColor = 0.8f;
    }
    
    // BRDF : Cook-Torrance Specular BRDF / Radiance 계산은 여기 함수 내부에서 처리할예정 
    float3 CT_BRDF = MY_BRDF_Irradiance(fRoughness, fMetallic, vAlbedo.rgb, F0, fAO, N, V, L, H);
    
    float3 vEmissive = g_EmissiveTarget.Sample(LinearSampler, In.vTexcoord).rgb;
    vEmissive = pow(vEmissive, gamma);
    
    Out.vColor.rgb = CT_BRDF * ShadowColor + vEmissive ;
    //Out.vColor.rgb = CT_BRDF * ShadowColor;
    Out.vColor.a = 1.f;
	
    return Out;
}
/*=============================================================
 
                         Technique 
                                
==============================================================*/

technique11 DefaultTechnique
{
    pass Debug // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Light_Spot // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOT();
    }

    pass Deferred // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

    pass PBR_Deferred // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PBR_DEFERRED();
    }

    pass New_PBR // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NEW_PBR();
    }
}