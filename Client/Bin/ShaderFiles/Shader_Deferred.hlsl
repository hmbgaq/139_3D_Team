#include "Shader_MyPBR.hlsl"
#include "DitherUtil.hlsli"
//#include "Shader_Defines.hlsli"
//#include "HeightFogUsage.hlsl"
//#pragma multi_compile _ HF_FOG_ENABLED HF_LIGHT_ATTEN

#define NUM_SAMPLES 4
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
    float   padding;            
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
vector  g_vLightDir;

// Spot
vector  g_vLightPos;
float   g_fLightRange;

// Point - g_vLightDir, g_vLightPos 둘도 사용
float   g_fRange;
float   g_fCutOff;
float   g_fOuterCutOff;

// Common 
bool    g_bPBR;
vector  g_vLightDiffuse;
vector  g_vLightAmbient;
vector  g_vLightSpecular;
float   g_fLightIntensity;

/* ========== Base ==========*/
matrix  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix  g_ProjMatrixInv, g_ViewMatrixInv;
matrix  g_LightViewMatrix, g_LightProjMatrix;
matrix  g_CamProjMatrix, g_CamViewMatrix; /* ssr에서 사용 */
vector  g_vCamPosition;
float   g_CamFar;
float   g_LightFar;

vector  g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector  g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);


/* ========== Texture2D ==========*/
Texture2D g_PriorityTarget;         /* Skybox - Priority */
Texture2D g_DiffuseTexture;         /* Out.Diffuse */ 
Texture2D g_NormalTexture;          /* Out.Normal */
Texture2D g_DepthTarget;            /* Out.Depth 카메라가 본 깊이 */ 
Texture2D g_ORMTexture;             /* Out.ORM */ 
Texture2D g_EmissiveTarget;         /* Emissive - For. PBR */
Texture2D g_ShadowDepthTexture;     /* Render_Shadow 결과 */
Texture2D g_SSAOTexture;            /* Pre - HBAO+ */
Texture2D g_BloomRimTarget;         /* BloomRim Blur */ 
Texture2D g_PerlinNoiseTexture;     /* Shor Fog - Noise Texture */
Texture2D g_Effect_DiffuseTarget;   /* Effect - Diffuse */
Texture2D g_OutlineTarget;          /* RenderGroup - Outline */
Texture2D g_ShadeTexture;           /* LightAcc - 빛이본 깊이 */
Texture2D g_SpecularTexture;        /* LightAcc - 빛이 본 Spec */
Texture2D g_AmbientTexture;         /* LightAcc - 각 빛의 pbr Ambient 계산해서 다 더한값 */
Texture2D g_VoxelReadTexture;
Texture2D g_ViewShadow;
Texture2D g_ShadowResult;

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

TextureCube g_IrradianceTexture;
TextureCube g_PreFilterTexture;
Texture2D   g_BRDFTexture;
float3      g_IrradianceColorScale;
float       g_fExposure;

/* ========== Cascade Shadow ============= */
float g_fStaticBias = 6.2;
float g_fShadowSizeRatio = 6.2;
matrix g_CascadeProj[3];
matrix g_StaticLightViewMatrix;

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
    float4 vSpecular    : SV_TARGET0;
    float4 vAmbient     : SV_TARGET1;
};

struct PS_OUT_FOG
{
    vector vFog : SV_TARGET0;
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

/* Directional Light */
float trowbridgeReitzNDF(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NdotH2 = NdotH * NdotH;
    float denom = ((alpha2 - 1) * NdotH2 + 1);
    float denominator = 3.141592265359 * denom * denom;
    return alpha2 / denominator;
}

float3 fresnel(float3 F0, float NdotV, float roughness)
{
    return F0 + (max(1.0 - roughness, F0) - F0) * pow(1 - NdotV, 5);
}

float schlickBeckmannGAF(float dotProduct, float roughness)
{
   // float alpha = roughness * roughness;
   // float k = alpha * 0.797884560803;  // sqrt(2 / PI)
    float fRoughness = (roughness + 1.f);
    float k = (fRoughness * fRoughness) / 8.f;
    
    return dotProduct / (dotProduct * (1 - k) + k);
}

//PCF
float PCF_StaticShadowCalculation_Cascade(float4 vWorldPos, float fBias, float fViewZ)
{
    float4 vProjCoords;

    float fShadow = 1.0f;

    fBias = g_fStaticBias;
    
    float2 texelSize = float2(1.f / 1280.0f, 1.f / 720.0f);

    if (fViewZ <= 10.0f) // 각각의 Cascade Target별로의 Shadow를 구하기 
    {
        vProjCoords = mul(vWorldPos, g_CascadeProj[0]);

        vProjCoords.x = vProjCoords.x * 0.5f + 0.5f;
        vProjCoords.y = vProjCoords.y * -0.5f + 0.5f;
        
        float currentDepth = vProjCoords.z;
        if (currentDepth > 1.0)
            return 1.0;

        currentDepth -= fBias;

        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                float pcfDepth = g_CascadeTarget1.Sample(PointClampSampler, vProjCoords.xy + float2(x, y) * texelSize).x;
                fShadow += currentDepth > pcfDepth ? 0.5f : 1.0f;
            }
        }
        fShadow /= 9.0f;

    }
    else if (fViewZ > 10.0f && fViewZ <= 20.0f)
    {
        vProjCoords = mul(vWorldPos, g_CascadeProj[1]);

        vProjCoords.x = vProjCoords.x * 0.5f + 0.5f;
        vProjCoords.y = vProjCoords.y * -0.5f + 0.5f;
        
        float currentDepth = vProjCoords.z;
        if (currentDepth > 1.0)
            return 1.0;

        currentDepth -= fBias;
        
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                float pcfDepth = g_CascadeTarget2.Sample(PointClampSampler, vProjCoords.xy + float2(x, y) * texelSize).x;
                fShadow += currentDepth > pcfDepth ? 0.5f : 1.0f;
            }
        }
        fShadow /= 9.0f;
    }
    else if (fViewZ > 20.0f)
    {
        vProjCoords = mul(vWorldPos, g_CascadeProj[2]);

        vProjCoords.x = vProjCoords.x * 0.5f + 0.5f;
        vProjCoords.y = vProjCoords.y * -0.5f + 0.5f;

        float2 vTexCoords = saturate(vProjCoords.xy);

        if (vTexCoords.x != vProjCoords.x || vTexCoords.y != vProjCoords.y)
            return 1.0f;

        float currentDepth = vProjCoords.z;
        if (currentDepth > 1.0)
            return 1.0;

        currentDepth -= fBias;

        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                float pcfDepth = g_CascadeTarget3.Sample(PointClampSampler, vProjCoords.xy + float2(x, y) * texelSize).x;
                fShadow += currentDepth > pcfDepth ? 0.5f : 1.0f;
            }
        }
        fShadow /= 9.0f;
    }
    else
    {
        return 1.0f;
    }

    return fShadow;
}

//PCF
float PCF_ShadowCalculation(float4 fragPosLightSpace, float fBias)
{
	// perform perspective divide
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// Transform to [0,1] range
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = projCoords.y * -0.5f + 0.5f;

    float2 vTexCoords = saturate(projCoords.xy);

    if (vTexCoords.x != projCoords.x || vTexCoords.y != projCoords.y)
        return 1.0f;

    float currentDepth = projCoords.z;
    if (currentDepth > 1.0)
        return 1.0;
	
	//fragPosLightSpace.w -= fBias;

    currentDepth -= fBias;

	// PCF
    float shadow = 0.0;
    float2 texelSize = float2(1.f / 1280.f, 1.f / 720.f);
    texelSize /= g_fShadowSizeRatio;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = g_ShadowDepthTexture.Sample(ShadowSampler, projCoords.xy + float2(x, y) * texelSize).x;
            shadow += currentDepth > pcfDepth ? 0.5f : 1.0f;
        }
    }
    shadow /= 9.0f;
    
    return shadow;
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
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

	/* 방향성광원의 정보와 노멀 타겟에 담겨있는 노멀과의 빛연산을 수행한다. */
    vector vDiffuseColor = g_DiffuseTexture.Sample(MIPSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(MIPSampler, In.vTexcoord);
    vector vDepthDesc = g_ShadowDepthTexture.Sample(MIPSampler, In.vTexcoord);
    vector vORMDesc = g_ORMTexture.Sample(MIPSampler, In.vTexcoord);
   
    /* Gamma 보정 */ 
    vDiffuseColor = pow(vDiffuseColor, 2.2f);
    vDiffuseColor.a = 1.f;
    
    float fMaxScalar = 0.f;
	
    /* 0 -> -1, 1 -> 1*/
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    vNormal = normalize(vNormal);
    float fViewZ = vDepthDesc.y * g_CamFar;
    
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.0f;

    /* 뷰스페이스 상 * 투영행렬까지 곱해놓은 위치를 구한다. */
    vWorldPos *= fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv); /* 뷰 스페이스 */ 
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv); /* 월드 스페이스 */

    vector vLook = normalize(vWorldPos - g_vCamPosition);
    vLook.a = 0.f;
    
    if (g_bPBR)
    {
        float fOcclusion = vORMDesc.x;
        float fRoughness = vORMDesc.y;
        float fMetalness = vORMDesc.z;
        
        fRoughness = max(fRoughness, 0.001f);
        fMetalness = max(fMetalness, 0.001f);
        fOcclusion = max(fOcclusion, 0.001f);
        
        vector vHalfVec = normalize(vLook + normalize(g_vLightDir) * -1.f);

        float NdotL = max(dot(vNormal, normalize(g_vLightDir) * -1.f), 0.0);
        float NdotH = max(dot(vNormal, vHalfVec), 0.0);
        float NdotV = max(dot(vNormal, vLook), 0.0);
        float HdotV = max(dot(vHalfVec, vLook), 0.0);

        float3 vMetalic = lerp(float3(0.04f, 0.04f, 0.04f), vDiffuseColor.xyz, fMetalness);

        float NDF = trowbridgeReitzNDF(NdotH, fRoughness);
        float3 F = fresnel(vMetalic, HdotV, fRoughness);
        float G = schlickBeckmannGAF(NdotV, fRoughness) * schlickBeckmannGAF(NdotL, fRoughness);

        vector kS = vector(F, 0.f);
        vector kD = vector(1.f, 1.f, 1.f, 0.f) - kS;
        kD *= 1.f - fMetalness;

        vector vNumerator = kS * NDF * G;
        float fDenominator = 4.f * NdotV * NdotL;
        vector vSpecular = vNumerator / max(fDenominator, 0.001f);

        vector vSpecularAcc = vSpecular * NdotL * g_vLightDiffuse * fOcclusion;
        vector vAmbientColor = vDiffuseColor * kD * NdotL * g_vLightDiffuse; /*/ 3.141592265359*/

        Out.vSpecular = vSpecularAcc * g_fLightIntensity;
        Out.vSpecular.a = 0.f;
        
        Out.vAmbient = vAmbientColor * g_fLightIntensity;
        Out.vAmbient.a = 1.f;
    }
    else
    { 
        
        //Out.vShade = g_vLightDiffuse * min((max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)), 1.f);
        Out.vAmbient = g_vLightDiffuse * min((max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)), 1.f);
        
        vector vLook = vWorldPos - g_vCamPosition;
        vector vReflect = reflect(normalize(g_vLightDir), vNormal);

        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);
    }
      
    return Out;
}

/* ------------------ 2 - Point ------------------ */
PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

	/* 방향성광원의 정보와 노멀 타겟에 담겨있는 노멀과의 빛연산을 수행한다. */
    vector vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTarget.Sample(LinearSampler, In.vTexcoord);
    vector vORMDesc = g_ORMTexture.Sample(LinearSampler, In.vTexcoord);
    vector vHBAO = g_SSAOTexture.Sample(LinearSampler, In.vTexcoord);
   // vLightFlagDesc *= g_vLightFlag;
	
    float fViewZ = vDepthDesc.y * g_CamFar;
	vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    vector vWorldPos;

	/* 투영스페이스 상의 위치르 ㄹ구한다. */
	/* 뷰스페이스 상 * 투영행렬 / w 까지 위치를 구한다. */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.0f;

	/* 뷰스페이스 상 * 투영행렬까지 곱해놓은 위치를 구한다. */
    vWorldPos *= fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    // Att
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    float fDenom = fDistance / g_fLightRange;
    float fAtt = 1.f / (1.f + fDenom * fDenom);
    clip(fAtt - 0.01f);
    fAtt *= g_fLightIntensity;
    
    //float fAtt = max((g_fLightRange - fDistance) / g_fLightRange, 0.f);
    
    vector vLook = normalize(g_vCamPosition - vWorldPos);
   
    float fRoughness = vORMDesc.y;
    float fMetalness = vORMDesc.z;
    float fOcclusion = vHBAO.x;
    
    if (false)
    {
        fRoughness = max(fRoughness, 0.001f);
        fMetalness = max(fMetalness, 0.001f);
        fOcclusion = max(fOcclusion, 0.001f);
    
        vector vHalfVec = normalize(vLook + normalize(vLightDir) * -1.f);

        float NdotL = max(dot(vNormal, normalize(vLightDir) * -1.f), 0.0);
        float NdotH = max(dot(vNormal, vHalfVec), 0.0);
        float NdotV = max(dot(vNormal, vLook), 0.0);
        float HdotV = max(dot(vHalfVec, vLook), 0.0);
    
        float3 vMetalic = lerp(float3(0.04f, 0.04f, 0.04f), vDiffuseColor.xyz, fMetalness);

        float NDF = trowbridgeReitzNDF(NdotH, fRoughness);
        float3 F = fresnel(vMetalic, HdotV, fRoughness);
        float G = schlickBeckmannGAF(NdotV, fRoughness) * schlickBeckmannGAF(NdotL, fRoughness);

        vector kS = vector(F, 0.f);
        vector kD = vector(1.f, 1.f, 1.f, 0.f) - kS;
        kD *= 1.f - fMetalness;

        vector vNumerator = kS * NDF * G;
        float fDenominator = 4.f * NdotV * NdotL;
        vector vSpecular = vNumerator / max(fDenominator, 0.001f);

        vector vSpecularAcc = vSpecular * NdotL * g_vLightDiffuse * fAtt /** fOcclusion*/;
        vector vAmbientColor = kD * vDiffuseColor / 3.141592265359 * fOcclusion * NdotL * g_vLightDiffuse * fAtt;

        Out.vSpecular = vSpecularAcc;
        Out.vSpecular.a = 0.f;

        Out.vAmbient = vAmbientColor;
        Out.vAmbient.a = 1.f;
      
    }
    else
    {
        vector vResult = g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
        
        if (vResult.r < 0.05f && vResult.g < 0.05f && vResult.b < 0.05f)
            discard;
        
        Out.vAmbient = vResult * fAtt;
        Out.vAmbient.a = 1.f;
        
        vector vReflect = reflect(normalize(vLightDir), vNormal);
        
        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), 20.f) * fAtt;
        Out.vSpecular.a = 0.f;
        
        /* 안하는경우 */
        //Out.vAmbient = g_vLightDiffuse * min((max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)), 1.f) * fAtt;
        //
        //vector vLook = vWorldPos - g_vCamPosition;
        //vector vReflect = reflect(normalize(vLightDir), vNormal);
        //
        //Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f) * fAtt;

    }
   
    //Out.vAmbient = pow(Out.vAmbient, 2.2f);
    //Out.vSpecular = pow(Out.vSpecular, 2.2f);
   
    return Out;
}

/* ------------------ 3 - Spot  ------------------ */
PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 1;
    
	/* 방향성광원의 정보와 노멀 타겟에 담겨있는 노멀과의 빛연산을 수행한다. */
    vector vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTarget.Sample(LinearSampler, In.vTexcoord);
    vector vORMDesc = g_ORMTexture.Sample(LinearSampler, In.vTexcoord);
    vector vHBAO = g_SSAOTexture.Sample(LinearSampler, In.vTexcoord);
    
    float fViewZ = vDepthDesc.y * g_CamFar;
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    vector vWorldPos;

	/* 투영스페이스 상의 위치르 ㄹ구한다. */
	/* 뷰스페이스 상 * 투영행렬 / w 까지 위치를 구한다. */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.0f;
    
    vWorldPos *= fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    // Att
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    float fDenom = fDistance / g_fLightRange;
    float fAtt = 1.f / (1.f + fDenom * fDenom);
    clip(fAtt - 0.01f);
    fAtt *= g_fLightIntensity;
    
    float fTheta = dot(normalize(vLightDir), normalize(g_vLightDir));
    
    float fEpsilon = max(0.f, g_fCutOff - g_fOuterCutOff);
    float fCutoffIntensity = (fTheta - g_fOuterCutOff) / fEpsilon;
    clip(fCutoffIntensity);
   
    fAtt *= fCutoffIntensity;
   
    vector vLook = normalize(g_vCamPosition - vWorldPos);
    
    //Out.vShade = vShadeDesc;
    float fRoughness = vORMDesc.y;
    float fMetalness = vORMDesc.z;
    float fOcclusion = vHBAO.x;

    if (g_bPBR)
    {
        fRoughness = max(fRoughness, 0.001f);
        fMetalness = max(fMetalness, 0.001f);
        fOcclusion = max(fOcclusion, 0.001f);
    
        vector vHalfVec = normalize(vLook + normalize(g_vLightDir) * -1.f);

        float NdotL = max(dot(vNormal, normalize(g_vLightDir) * -1.f), 0.0);
        float NdotH = max(dot(vNormal, vHalfVec), 0.0);
        float NdotV = max(dot(vNormal, vLook), 0.0);
        float HdotV = max(dot(vHalfVec, vLook), 0.0);
    
        float3 vMetalic = lerp(float3(0.04f, 0.04f, 0.04f), vDiffuseColor.xyz, fMetalness);

        float NDF = trowbridgeReitzNDF(NdotH, fRoughness);
        float3 F = fresnel(vMetalic, HdotV, fRoughness);
        float G = schlickBeckmannGAF(NdotV, fRoughness) * schlickBeckmannGAF(NdotL, fRoughness);

        vector kS = vector(F, 0.f);
        vector kD = vector(1.f, 1.f, 1.f, 0.f) - kS;
        kD *= 1.f - fMetalness;

        vector vNumerator = kS * NDF * G;
        float fDenominator = 4.f * NdotV * NdotL;
        vector vSpecular = vNumerator / max(fDenominator, 0.001f);

        vector vSpecularAcc = vSpecular * NdotL * g_vLightDiffuse * fAtt;
        vector vAmbientColor = kD * vDiffuseColor / 3.141592265359 * fOcclusion * NdotL * g_vLightDiffuse * fAtt;


        Out.vSpecular = vSpecularAcc;
        Out.vSpecular.a = 0.f;
        
        Out.vAmbient = vAmbientColor;
        Out.vAmbient.a = 1.f;
      
    }
    else
    {
        vector vResult = g_vLightDiffuse * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

        if (vResult.r < 0.05f && vResult.g < 0.05f && vResult.b < 0.05f)
            discard;

        Out.vAmbient = vResult * fAtt;
        Out.vAmbient.a = 1.f;

        vector vReflect = reflect(normalize(g_vLightDir), vNormal);
   
        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), 20.f) * fAtt;
        Out.vSpecular.a = 0.f;
    }
    
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
        
        Out.vColor = vPriority;
        return Out;
    }
    
    // MRT_LightAcc : Ambient  
    vector vAmbient = g_AmbientTexture.Sample(LinearSampler, In.vTexcoord);
    vAmbient = saturate(vAmbient);
	
    // MRT_LightAcc : Specular 
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    vSpecular = saturate(vSpecular);
    
    // MRT_GameObject : Emissive 
    vector vEmissive = g_EmissiveTarget.Sample(LinearSampler, In.vTexcoord);
    
    // Target_HBAO
    vector vSSAO = float4(1.f, 1.f, 1.f, 1.f);
    if (g_bSSAO_Active)
        vSSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = (vDiffuse * vAmbient * vSSAO) + vSpecular + vEmissive;
    //Out.vColor = (vDiffuse * vSSAO) + vSpecular;
    
    if (true == g_bShadow_Active)
    {
        vector Shadow = g_ShadowResult.Sample(LinearSampler, In.vTexcoord);
        
        Out.vColor = Out.vColor * Shadow;
    }
    
    Out.vColor.a = 1.f;
    
    return Out;
}

/* ------------------ 5 - PBR Deferred ------------------ */
PS_OUT PS_MAIN_PBR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) Out;

    vector vDiffuse     = g_DiffuseTexture.Sample(MIPSampler, In.vTexcoord);
    if(vDiffuse.a == 0 )
    {
        float4 vPriority = g_PriorityTarget.Sample(LinearSampler, In.vTexcoord);
        if (vPriority.a == 0.f)
            discard;
        
        Out.vColor = vPriority;
        
        return Out;
    }
    vector vNormalDesc  = g_NormalTexture.Sample(MIPSampler, In.vTexcoord);
    vector vSpecular    = g_SpecularTexture.Sample(MIPSampler, In.vTexcoord);
    vector vAmbientDesc = g_AmbientTexture.Sample(MIPSampler, In.vTexcoord);
    vector vViewShadow  = g_ViewShadow.Sample(MIPSampler, In.vTexcoord);
    vector vDepthDesc   = g_DepthTarget.Sample(MIPSampler, In.vTexcoord);
    vector vORMDesc     = g_ORMTexture.Sample(MIPSampler, In.vTexcoord);
    vector vHBAO        = g_SSAOTexture.Sample(MIPSampler, In.vTexcoord);
    
    float fViewZ = vDepthDesc.y * g_CamFar;

    vector vWorldPos;

	/* 투영스페이스 상의 위치르 ㄹ구한다. */
	/* 뷰스페이스 상 * 투영행렬 / w 까지 위치를 구한다. */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.0f;

	/* 뷰스페이스 상 * 투영행렬까지 곱해놓은 위치를 구한다. */
    vWorldPos *= fViewZ;

	/* 뷰스페이스 상  위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드페이스 상  위치를 구한다. */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
        
    float fCamDistance = length(vWorldPos - g_vCamPosition);
    
    bool bIsInShadow = 0.9f > vViewShadow.r;
  
    if (vORMDesc.r > 0.f || vORMDesc.g > 0.f || vORMDesc.b > 0.f)
    {
        vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
        vNormal = normalize(vNormal);
        vector vLook = normalize(g_vCamPosition - vWorldPos);
    
        float NdotV = max(dot(vNormal, vLook), 0.0);
    
        float fRoughness = max(0.01f, vORMDesc.y);
        float fMetalness = max(0.01f, vORMDesc.z);
        float fOcclusion = max(0.01f, vORMDesc.x);
     
        vector vReflect = normalize(reflect(-normalize(vLook), vNormal));
        
        float4 brdfTerm = g_BRDFTexture.Sample(ClampSampler, float2(NdotV, 1.0 - fRoughness));
        float3 metalSpecularIBL = g_PreFilterTexture.SampleLevel(ClampSampler, vReflect.xyz, (fRoughness * 10.f)).rgb;

        float3 dielectricColor = float3(0.04, 0.04, 0.04);
        float3 diffColor = vDiffuse.rgb * (1.0 - fMetalness);
        float3 specColor = lerp(dielectricColor.rgb, vDiffuse.rgb, fMetalness);
   
        vector diffuseIBL = g_IrradianceTexture.Sample(ClampSampler, vNormal) / 3.14159;
        diffuseIBL.rgb = lerp(diffuseIBL.rgb * 0.3f, diffuseIBL.rgb, fOcclusion);
    
        float3 albedoByDiffuse = diffColor.rgb * diffuseIBL.rgb;

        float4 litColor;
        litColor.rgb = (albedoByDiffuse.rgb + (metalSpecularIBL * (specColor * brdfTerm.x + (brdfTerm.y)))) * fOcclusion;
 
        litColor = pow(litColor, 2.2f);
      
        Out.vColor.rgb = vAmbientDesc.rgb + vSpecular.rgb + (litColor.rgb * g_IrradianceColorScale);
        Out.vColor.rgb *= vViewShadow.rgb;
        Out.vColor.rgb *= vHBAO.rgb;
           
        Out.vColor.a = 1.f;
        vector vSpecular = g_SpecularTexture.Sample(MIPSampler, In.vTexcoord);
        vector vAmbientDesc = g_AmbientTexture.Sample(MIPSampler, In.vTexcoord);
    }
    else
    {
        Out.vColor = vDiffuse * vAmbientDesc + vSpecular /** (1.f - bIsInShadow)*/;
        Out.vColor.rgb *= vViewShadow.rgb;
    }
    
    float3 mapped = 1.f - exp(-Out.vColor.rgb * g_fExposure);
    Out.vColor.rgb = pow(mapped, 1.f / 2.2f);
    
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
        
        Out.vColor = vPriority;
        
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
    float3 L = normalize(-g_vLightDir.xyz);
    float3 H = normalize(V + L);
        
    // BRDF : Cook-Torrance Specular BRDF / Radiance 계산은 여기 함수 내부에서 처리할예정 
    float3 CT_BRDF = MY_BRDF_Irradiance(fRoughness, fMetallic, vAlbedo.rgb, F0, fAO, N, V, L, H);
    
    // - 3. Emissive ----------------
    float3 vEmissive = g_EmissiveTarget.Sample(LinearSampler, In.vTexcoord).rgb;
    vEmissive = pow(vEmissive, gamma);
    
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    vector vAmbientDesc = g_AmbientTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vShadow = g_ShadowResult.Sample(LinearSampler, In.vTexcoord);
    
    float4 ColorCombine = float4(CT_BRDF + vEmissive,  1.f);
    ColorCombine += vSpecular + vAmbientDesc;
    ColorCombine += vAmbientDesc;
    
    if (vShadow.r > 0.f)
    {
        Out.vColor = ColorCombine * vShadow; // 그림자 효과 적용
    }
    else
    {
        Out.vColor = ColorCombine;
    }
    
    Out.vColor = ColorCombine;
    
    if(Out.vColor.a == 0 )
        discard;
    
    return Out;
}

/* ------------------ 7 - ViewShadow ------------------ */
PS_OUT PS_MAIN_VIEW_SHADOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = float4(1.f, 1.f, 1.f, 1.f); // 흰색 
    
    vector vDepth = g_DepthTarget.Sample(MIPSampler, In.vTexcoord); /* GameObject 의 Depth */
    float fViewZ = vDepth.y * g_CamFar;
    
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepth.x;
    vWorldPos.w = 1.0f;

	/* 뷰스페이스 상 * 투영행렬까지 곱해놓은 위치를 구한다. */
    vWorldPos *= fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
	
    vector vStaticPosition = vWorldPos;
    vStaticPosition = mul(vStaticPosition, g_LightViewMatrix);
    vStaticPosition = mul(vStaticPosition, g_LightProjMatrix);
    
    float2 vNewUV = float2(0.f, 0.f);
    vNewUV.x = (vStaticPosition.x / vStaticPosition.w) * 0.5f + 0.5f;
    vNewUV.y = (vStaticPosition.y / vStaticPosition.w) * -0.5f + 0.5f;
	
    /* 기존코드 */ 
    vector vShadowDepth = g_ShadowDepthTexture.Sample(LinearSampler, vNewUV);
    
    if (vStaticPosition.w - 0.1f > vShadowDepth.x * g_LightFar)
        Out.vColor = 0.7f;

    // PCF 샘플 개수와 오프셋 정의
   //if (vStaticPosition.w - 0.1f > vShadowDepth.x * g_LightFar)
   //{
   //    float sampleOffsets[NUM_SAMPLES] = { -0.5f, 0.5f, -0.5f, 0.5f }; // 예시로 간단하게 4개의 샘플 사용
   //
   //// 그림자 강도 계산
   //    float shadowAmount = 0.0f;
   //    for (int i = 0; i < NUM_SAMPLES; ++i)
   //    {
   //        float2 offsetUV = vNewUV + float2(sampleOffsets[i], sampleOffsets[i]);
   //        vector vSampleShadowDepth = g_ShadowDepthTexture.Sample(LinearSampler, offsetUV);
   //        shadowAmount += (vStaticPosition.w - 0.1f > vSampleShadowDepth.x * g_LightFar) ? 1.0f : 0.0f;
   //    }
   //    shadowAmount /= float(NUM_SAMPLES); // 그림자 강도 정규화
   //
   //// 그림자 적용
   //    Out.vColor = lerp(Out.vColor, float4(0.7f, 0.7f, 0.7f, 1.0f), shadowAmount); // PCF 적용
   //}
    
    return Out;
}

/* ------------------ 8 - Fog ------------------ */
PS_OUT_FOG PS_MAIN_FOG(PS_IN In)
{
    PS_OUT_FOG Out = (PS_OUT_FOG) 1;

    /* 방향성광원의 정보와 노멀 타겟에 담겨있는 노멀과의 빛연산을 수행한다. */
    vector vDepthDesc = g_DepthTarget.Sample(MIPSampler, In.vTexcoord);

    float fViewZ = vDepthDesc.y * g_CamFar;
    /* 0 -> -1, 1 -> 1*/

    vector vWorldPos;

    /* 투영스페이스 상의 위치르 ㄹ구한다. */
    /* 뷰스페이스 상 * 투영행렬 / w 까지 위치를 구한다. */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.0f;

    /* 뷰스페이스 상 * 투영행렬까지 곱해놓은 위치를 구한다. */
    vWorldPos *= fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    /* 기존 Deferred에서 쓰던 코드 */ 
    float3 vTexCoord = float3((vWorldPos.xyz * 100.f) % 12800.f) / 12800.f;
    vTexCoord.x += g_vFogUVAcc.x;
    vTexCoord.y += g_vFogUVAcc.y;
    
    float fNoise = g_PerlinNoiseTexture.Sample(LinearSampler, vTexCoord.xy).r;
    
    float4 Origincolor = { 0.f, 0.f, 0.f, 0.f };
    float3 vFinalColor = Compute_HeightFogColor(Origincolor.rgb, (vWorldPos - g_vCamPosition).xyz, fNoise, g_Fogdesc);
        
    //vector vFogDir = vWorldPos - g_vCamPosition;
    //float fDistance = length(vFogDir);
    //float fAtt = saturate((g_fFogRange - fDistance) / g_fFogRange);    
    //Out.vFog = (g_vFogColor.a - (fAtt * fAtt));
   
    Out.vFog = vector(vFinalColor.rgb, 1.f);
    
    return Out;
}

/* ------------------ 8 - Fog ------------------ */
PS_OUT PS_MAIN_TEST(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
 
    vector vDiffuseColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    if (vDiffuseColor.a == 0)
        discard;
    
    Out.vColor = vDiffuseColor;
    
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
        SetBlendState(BS_AlphaBlending, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlending, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Light_Spot // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlending, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);
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
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PBR();
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

    pass ViewShadow // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_VIEW_SHADOW();
    }

    pass Fog //8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FOG();
    }

    pass Test //9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TEST();
    }

}