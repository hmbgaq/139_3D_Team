#include "Shader_Defines.hlsli"
/* -------------- 공 지 사 항 ------------------- */
// 
//
//
/* ----------------------------------------- */

/* Base */
matrix    g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix    g_BoneMatrices[800];
float4    g_vCamPosition;
float     g_fCamFar;
float     g_fLightFar;
float     g_TimeDelta;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_SpecularTexture;
Texture2D g_DissolveTexture;
Texture2D g_MaskingTexture;

/* =========== Value =========== */
float   g_fDissolveWeight;    /* Dissolve  */

float4  g_vLineColor;        /* OutLine */
float   g_LineThick;          /* OutLine */

float3  g_vBloomPower = { 0.f, 0.f, 0.f }; /* Bloom */
float4  g_vRimColor = { 0.f, 0.f, 0.f, 0.f }; /* RimLight */
float   g_fRimPower = 5.f; /* RimLight */
/* ------------------- function ------------------- */ 
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
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vORM : SV_TARGET3;
    float4 vRimBloom : SV_TARGET4; /* Rim + Bloom */
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
 
    /* ---------------- New ---------------- */
    // float4 vRimColor = Calculation_RimColor(In.vNormal, In.vPosition);
    // Out.vDiffuse += vRimColor;
    // Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) + vRimColor;
    
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
PS_OUT PS_BOSS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
 
    /* ---------------- New ---------------- */
    float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    Out.vDiffuse += vRimColor;
   // Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) + vRimColor;
    /* g_vBloomPower */

    // Out.vDiffuse += vRimColor; // 효과 약하게 하고싶으면 Bloom에 넣지말고 여기에 넣기 
    return Out;
}

/* ------------------- Pixel Shader(4) -------------------*/
PS_OUT PS_MAIN_RIMBLOOM_A(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a == 0.f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
 
    /* ---------------- New ---------------- */
    float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) + vRimColor;
    
    
    // Out.vDiffuse += vRimColor; // 효과 약하게 하고싶으면 Bloom에 넣지말고 여기에 넣기 
    return Out;
}
/* ------------------- Pixel Shader(5) -------------------*/
PS_OUT PS_MAIN_RIMBLOOM_B(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a == 0.f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
 
    /* ---------------- New ---------------- */
    float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) ;
    
    
    // Out.vDiffuse += vRimColor; // 효과 약하게 하고싶으면 Bloom에 넣지말고 여기에 넣기 
    return Out;
}
/* ------------------- Pixel Shader(6) -------------------*/
PS_OUT PS_MAIN_RIMBLOOM_C(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a == 0.f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
 
    /* ---------------- New ---------------- */
    float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    Out.vDiffuse += vRimColor;
    Out.vRimBloom = Calculation_Brightness(Out.vDiffuse);
    
    
    // Out.vDiffuse += vRimColor; // 효과 약하게 하고싶으면 Bloom에 넣지말고 여기에 넣기 
    return Out;
}
/* ------------------- Pixel Shader(7) -------------------*/
PS_OUT PS_MAIN_RIMBLOOM_D(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a == 0.f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
 
    /* ---------------- New ---------------- */
    float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    Out.vDiffuse += vRimColor;
    Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) + vRimColor;
    
    
    // Out.vDiffuse += vRimColor; // 효과 약하게 하고싶으면 Bloom에 넣지말고 여기에 넣기 
    return Out;
}

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

    pass BossModel // 3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BOSS();
    }

    pass RimBloom_A // 4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RIMBLOOM_A();
    }

    pass RimBloom_B // 4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RIMBLOOM_B();
    }
    pass RimBloom_C // 4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RIMBLOOM_C();
    }
    pass RimBloom_D // 4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RIMBLOOM_D();
    }
}