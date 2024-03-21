
#include "Shader_PBR.hlsl"
//#include "Shader_Defines.hlsli"
//#include "HeightFogUsage.hlsl"
//#pragma multi_compile _ HF_FOG_ENABLED HF_LIGHT_ATTEN

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ProjMatrixInv, g_ViewMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;
matrix g_CamProjMatrix, g_CamViewMatrix; /* ssr에서 사용 */
float g_CamFar;
float g_LightFar;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;
float g_fLightIntensity;
vector g_vLightFlag;

vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

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

/* 활성 여부 */
bool g_bSSAO_Active;
bool g_bFog_Active;
bool g_bShadow_Active;

/* 안개 */
float2  g_vFogUVAcc             = { 0.f, 0.f };

/* PBR */
float g_fBias;


struct FOG_DESC 
{
    bool  bFog_Active;
    float fFogStartDepth;
    float fFogStartDistance;
    float fFogDistanceValue;
    float fFogHeightValue;
    float fFogDistanceDensity;
    float fFogHeightDensity;
    float3 padding; // 12바이트 패딩
    float4 vFogColor;
};

struct BLOOMRIM_DESC
{
    // 패딩을 추가하지않아도 메모리가 정렬됨 
    bool bBloomBlur_Active; 
    bool bRimBlur_Active; 
};

FOG_DESC g_Fogdesc;
BLOOMRIM_DESC g_Bloom_Rim_Desc;
/* ------------------ Function ------------------ */ 

float3 Compute_HeightFogColor(float3 vOriginColor, float3 toEye, float fNoise, FOG_DESC desc)
{
    /* 
    vOriginColor : 안개없이 원래 그리는 색상 
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


/* ------------------ ------------------ */ 
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

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* In.vPosition * 월드 * 뷰 * 투영 */
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* ------------------ 0 - DEBUG ------------------ */

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

/* ------------------ 1 - Directional ------------------ */

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDiffuseColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
	//vector		
	
    vDiffuseColor = pow(vDiffuseColor, 2.2f);
    vDiffuseColor.a = 1.f;
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

	/* 방향성광원의 정보와 노멀 타겟에 담겨있는 노멀과의 빛연산을 수행한다. */
    vector vDiffuseColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
    vector vORMDesc = g_ORMTexture.Sample(PointSampler, In.vTexcoord);

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
        return Out;
    }
    // MRT_LightAcc : Shade 
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vShade = saturate(vShade);
	
    // MRT_GameObject : Specular 
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    vSpecular = saturate(vSpecular);
	
    // MRT_GameObject : Depth
    vector vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
    if (vDepthDesc.w != 1.f) 
        vSpecular = float4(0.f, 0.f, 0.f, 0.f);
    
    // Target_HBAO
    vector vSSAO = float4(1.f, 1.f, 1.f, 1.f);
    if (g_bSSAO_Active)
        vSSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = (vDiffuse * vShade * vSSAO) + vSpecular;
    
    float fViewZ = vDepthDesc.y * g_CamFar;
	
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
        
    if (true == g_bFog_Active )
    {
        float3 vTexCoord = float3((vWorldPos.xyz * 100.f) % 12800.f) / 12800.f;
        vTexCoord.x += g_vFogUVAcc.x;
        vTexCoord.y += g_vFogUVAcc.y;
    
        float fNoise = g_PerlinNoiseTexture.Sample(LinearSampler, vTexCoord.xy).r;
    
        float3 vFinalColor = Compute_HeightFogColor(Out.vColor.xyz, (vWorldPos - g_vCamPosition).xyz, fNoise, g_Fogdesc);
    
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
    
    vAlbedo = pow(vAlbedo, 2.2f);
    vector vNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 N = vNormal.xyz * 2.f - 1.f;
    vector vORMDesc = g_ORMTexture.Sample(LinearSampler, In.vTexcoord); /* (R)Roughness ,(G)Metallic , (B)Ambient Occlusion */
    
    float fRoughness = vORMDesc.r;
    float fMetallic = vORMDesc.g;
    float fAmbient_Occlusion = vORMDesc.b;
    
    vector vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
    
    float fAO = 1.f;
    
    fAO = g_SSAOTexture.Sample(LinearSampler, In.vTexcoord).r;
    
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
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, vAlbedo.xyz, fMetallic); // 반사율 F0

	// calculate per-light radiance
    float3 L = normalize(-g_vLightDir);
    float3 H = normalize(V + L);

    Out.vColor.rgb = BRDF(fRoughness, fMetallic, vAlbedo.xyz, F0, N, V, L, H, fAO);
    
    // Shadow 
    float ShadowColor = 1.f;
    
    if (true == g_bShadow_Active) 
    {
        //float fDot = saturate(dot(normalize(g_vLightDir.xyz) * -1.f, vNormal.xyz));
        //
        //float fNormalOffset = g_fBias;
        //float fBias = max((fNormalOffset * 5.0f) * (1.0f - (fDot * -1.0f)), fNormalOffset);
   
        vector vPosition = mul(vWorldPos, g_LightViewMatrix);
        vPosition = mul(vPosition, g_LightProjMatrix);
   
        float2 vUV = (float2) 0.0f;
   
        vUV.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
        vUV.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
   
        float4 vLightDepth = g_ShadowDepthTexture.Sample(LinearSampler, vUV);
   
        if (vPosition.w - 0.1f > vLightDepth.x * g_LightFar) /* LightFar */ 
            Out.vColor = Out.vColor * 0.8f;
    }
    
    if (true == g_bFog_Active)
    {
        float3 vTexCoord = float3((vWorldPos.xyz * 100.f) % 12800.f) / 12800.f;
        vTexCoord.x += g_vFogUVAcc.x;
        vTexCoord.y += g_vFogUVAcc.y;
    
        float fNoise = g_PerlinNoiseTexture.Sample(LinearSampler, vTexCoord.xy).r;
    
        float3 vFinalColor = Compute_HeightFogColor(Out.vColor.xyz, (vWorldPos - g_vCamPosition).xyz, fNoise, g_Fogdesc);
    
        Out.vColor = vector(vFinalColor.rgb, 1.f);
    }
    
    //Out.vColor += vEffect;
    Out.vColor.a = 1.f;
	
    return Out;
}

/* ------------------ Technique ------------------ */

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
}