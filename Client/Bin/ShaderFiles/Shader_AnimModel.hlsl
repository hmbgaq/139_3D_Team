#include "Shader_Defines.hlsli"
/* -------------- 필 독 사 항 ------------------- */
// 
// Bloom , Blur, RimLight 사용법 : 노션 - 개인페이지 Shader사용법 */ 
//
/* ----------------------------------------- */

/* Base */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_BoneMatrices[800];
float g_fCamFar;
float g_fLightFar;
float g_TimeDelta;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_SpecularTexture;
Texture2D g_DissolveTexture;
Texture2D g_MaskingTexture;

/* =========== Value =========== */
float g_fDissolveWeight; /* Dissolve  */

float4 g_BloomColor = { 0.f, 0.f, 0.f, 0.f }; /* Bloom */

matrix g_ReflectionMatrix; /* Reflection */

float4 g_vLineColor; /* OutLine */
float g_LineThick; /* OutLine */

float4 g_vRimColor = { 0.f, 0.f, 0.f, 0.f }; /* RimLight */
float3 g_vRimPower; /* RimLight */
float4 g_vCamPosition; /* RimLight */

/* ------------------- function ------------------- */ 
float4 Calculation_RimColor(float4 In_Normal, float4 In_Pos)
{
    float fRimPower = 1.f - saturate(dot(In_Normal, normalize((-1.f * (In_Pos - g_vCamPosition)))));
    fRimPower = pow(fRimPower, 5.f);
    float4 vRimColor = g_vRimColor * fRimPower;
    
    return vRimColor;
}

float4 Calculation_Brightness(float4 Out_Diffuse)
{
    float4 vBrightnessColor = float4(0.f, 0.f, 0.f, 0.f);

    float fPixelBrightness = dot(Out_Diffuse.rgb, g_vRimPower.rgb);
    
    if (fPixelBrightness > 0.99f)
        vBrightnessColor = float4(Out_Diffuse.rgb, 1.0f);

    return vBrightnessColor;
}

/* ------------------- ------------------- */ 

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse     : SV_TARGET0;
    float4 vNormal      : SV_TARGET1;
    float4 vDepth       : SV_TARGET2;
    float4 vORM         : SV_TARGET3;
    float4 vBloomBlur   : SV_TARGET4;
    float4 vRimBlur     : SV_TARGET5;
  
};

/* ------------------- Base Vertex Shader -------------------*/

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		                g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		                g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		                g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
	
    return Out;
}

/* ------------------- Base Pixel Shader (0) -------------------*/

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a == 0.f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f); 
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
 
    Out.vBloomBlur = Calculation_Brightness(Out.vDiffuse);
   // float4 vRimColor = Calculation_Brightness(Out.vDiffuse) + Calculation_RimColor(In.vNormal, In.vPosition); /* g_vRimPowerm ,g_vRimColor , g_vCamPosition 사용 */
   // Out.vRimBlur = vRimColor; /* g_vRimPower 사용 */
	
    return Out;
}   

/* ------------------- Shadow Pixel Shader(2) -------------------*/

struct PS_OUT_SHADOW
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    Out.vLightDepth = In.vProjPos.w / g_fLightFar;
	
    return Out;
}
/* ------------------- Pixel Shader(3) -------------------*/


/* ------------------- Technique -------------------*/ 
technique11 DefaultTechnique
{
    pass Model // 0
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Blend // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Shadow // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

}
