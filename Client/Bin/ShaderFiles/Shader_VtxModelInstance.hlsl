#include "Shader_Defines.hlsli"

/*=============================================================
 
                             Value
                                
==============================================================*/
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vCamPosition;
float g_fCamFar;
float g_fLightFar;
float g_fTimeDelta;
vector g_vCamLook;
vector g_vPlayerPosition;
float g_fDissolveRatio;
float g_DiscardAlpha = 0.1f;
bool g_bORM_Available;
bool g_bEmissive_Available;

/* =========== Texture =========== */
Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_SpecularTexture;
Texture2D g_EmissiveTexture;
Texture2D g_OpacityTexture;
Texture2D g_AmbientOcclusionTexture;
Texture2D g_RoughnessTexture;
Texture2D g_MetalicTexture;

Texture2D g_MaskTexture;
Texture2D g_DissolveTexture;
Texture2D g_DissolveDiffTexture;

/* =========== Shader Value =========== */
float     g_fDissolveWeight;                    /* Dissolve  */

float4    g_vLineColor;                         /* OutLine */
float     g_LineThick;                          /* OutLine */

float3    g_vBloomPower = { 0.f, 0.f, 0.f };    /* Bloom */
float4    g_vRimColor = { 0.f, 0.f, 0.f, 0.f }; /* RimLight */
float     g_fRimPower = 5.f;                    /* RimLight */

/*=============================================================
 
                           Function 
                                
==============================================================*/

float4 Calculation_RimColor(float4 In_Normal, float4 In_Pos)
{
    float fRimPower = 1.f - saturate(dot(In_Normal, normalize((-1.f * (In_Pos - g_vCamPosition)))));
    fRimPower = pow(fRimPower, g_fRimPower); // 여기서 강도를 조정한다. 
    
    float4 vRimColor = g_vRimColor * fRimPower;
    return vRimColor;
}

float4 Calculation_Brightness(float4 Out_Diffuse)
{
    float4 vBrightnessColor = float4(0.f, 0.f, 0.f, 0.f);

    float fPixelBrightness = dot(Out_Diffuse.rgb, g_vBloomPower.rgb);
    
    if (fPixelBrightness > 0.99f)
        vBrightnessColor = float4(Out_Diffuse.rgb, 1.0f);

    return vBrightnessColor;
}

float IsIn_Range(float fMin, float fMax, float fValue)
{
    return (fMin <= fValue) && (fMax >= fValue);
}

/*=============================================================
 
                             Struct
                                
==============================================================*/ 

struct VS_IN
{
    float3 vPosition    : POSITION;
    float3 vNormal      : NORMAL;
    float2 vTexUV       : TEXCOORD0;
    float3 vTangent     : TANGENT;

    float4 vRight       : TEXCOORD1;
    float4 vUp          : TEXCOORD2;
    float4 vLook        : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;

};

struct VS_OUT
{
    float4 vPosition    : SV_POSITION;
    float4 vNormal      : NORMAL;
    float2 vTexUV       : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
    float4 vTangent     : TANGENT;
    float4 vBinormal    : BINORMAL;
};

struct VS_OUT_SHADOW
{
    float4 vPosition    : SV_POSITION;
    float4 vProjPos     : TEXCOORD0;
};

struct PS_IN
{
    float4 vPosition    : SV_POSITION;
    float4 vNormal      : NORMAL;
    float2 vTexUV       : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
    float4 vTangent     : TANGENT;
    float4 vBinormal    : BINORMAL;
};

struct PS_OUT
{
    float4 vDiffuse     : SV_TARGET0;
    float4 vNormal      : SV_TARGET1;
    float4 vDepth       : SV_TARGET2;
    float4 vORM         : SV_TARGET3;
    float4 vRimBloom    : SV_TARGET4; /* Rim + Bloom */
    float4 vEmissive    : SV_Target5;
};

struct PS_IN_SHADOW
{
    float4 vPosition    : SV_POSITION;
    float4 vProjPos     : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
    vector vLightDepth  : SV_TARGET0;
};

/*=============================================================
 
                        Vertex Shader
                                
==============================================================*/
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix WorldMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    vector vPosition = mul(vector(In.vPosition, 1.f), WorldMatrix);
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition   = mul(vPosition, matWVP);
    Out.vNormal     = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vWorldPos   = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTexUV      = In.vTexUV;
    Out.vProjPos    = Out.vPosition;
    Out.vTangent    = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal   = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

    return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;


    matrix WorldMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    vector vPosition = mul(vector(In.vPosition, 1.f), WorldMatrix);
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vProjPos = Out.vPosition;

    return Out;
}
/*=============================================================
 
                        Pixel Shader
                                
==============================================================*/
/* ------------------- Base Pixel Shader (0) -------------------*/
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    //if (vMtrlDiffuse.a <= g_DiscardAlpha)
    //    discard;
    
    clip(vMtrlDiffuse.a - 0.1f);
	
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
    vPixelNormal    = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal    = mul(vPixelNormal, WorldMatrix);
    
    Out.vDiffuse    = vMtrlDiffuse;
    Out.vNormal     = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth      = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM        = float4(0.f, 0.f, 0.f, 0.f);
    Out.vEmissive   = float4(0.f, 0.f, 0.f, 0.f);
    
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
    
    return Out;
}

/* ------------------- (1) -------------------*/
PS_OUT PS_MAIN_NORMAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    clip(Out.vDiffuse.a - 0.1f);
    
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal = mul(vPixelNormal, WorldMatrix);

    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	
    return Out;
}

/* ------------------- (2) -------------------*/
PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    Out.vLightDepth.r = In.vProjPos.w / g_fLightFar;

    Out.vLightDepth.a = 1.f;

    return Out;
}

/* ------------------- (3) -------------------*/
// NoneCull
/* ------------------- (4) -------------------*/
// NoneCull

/* ------------------- (5) -------------------*/
PS_OUT PS_MAIN_Dissove(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector TexDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);

    clip(TexDissolve - g_fDissolveRatio);

    vector vTexDiff = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    float fStepValue = IsIn_Range(0.f, 0.05f, TexDissolve.r - g_fDissolveRatio);
	
    Out.vDiffuse = (1.f - fStepValue) * vTexDiff + fStepValue * g_DissolveDiffTexture.Sample(LinearSampler, In.vTexUV);
	
    clip(Out.vDiffuse.a - 0.1f);
    Out.vDiffuse.a = 1.f;
	
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal = mul(vPixelNormal, WorldMatrix);
	
    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);

    return Out;
}

/* ------------------- (6) -------------------*/
PS_OUT PS_MAIN_NORMALNONCLIP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    if (Out.vDiffuse.a <= 0.0f)
        discard;
		
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal = mul(vPixelNormal, WorldMatrix);

    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
	
    return Out;
}

/*=============================================================
 
                          Technique
                                
==============================================================*/
technique11 DefaultTechnique
{
    pass Default //0
    {
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NormalMapping //1
    {
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
    }

    pass ShadowDepth //2
    {
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_None, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass Pass4_NonCulling //3
    {
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Pass5_NonCulling_Norm //4
    {
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
    }

    pass Pass6_Dissolve //5
    {
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Dissove();
    }

    pass NormalMappingNonClip //6
    {
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMALNONCLIP();
    }

}