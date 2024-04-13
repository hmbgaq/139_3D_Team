#include "Shader_Defines.hlsli"
/*=============================================================
 
                             Value
                                
==============================================================*/

/* =========== Common =========== */
matrix    g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix    g_BoneMatrices[800];
float4    g_vCamPosition;
float     g_fCamFar;
float     g_fLightFar;
float     g_TimeDelta;
float4    g_vDirectionalDir;

bool      g_bORM_Available;
bool      g_bEmissive_Available;

/* =========== Texture =========== */
Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_SpecularTexture;
Texture2D g_EmissiveTexture;
Texture2D g_OpacityTexture;

Texture2D g_DissolveTexture;
Texture2D g_MaskingTexture;

/* =========== Value =========== */
float   g_fDissolveWeight;      /* Dissolve  */

float4  g_vLineColor;           /* OutLine */
float   g_LineThick;            /* OutLine */

float3  g_vBloomPower   = { 0.f, 0.f, 0.f };        /* Bloom */
float4  g_vRimColor     = { 0.f, 0.f, 0.f, 0.f };   /* RimLight */
float   g_fRimPower     = 5.f;                      /* RimLight */

matrix g_CascadeProj; /* Cascade */
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

/*=============================================================
 
                             Struct
                                
==============================================================*/

struct VS_IN
{
    float3  vPosition       : POSITION;
    float3  vNormal         : NORMAL;
    float2  vTexcoord       : TEXCOORD0;
    float3  vTangent        : TANGENT;
    uint4   vBlendIndices   : BLENDINDEX;
    float4  vBlendWeights   : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition        : SV_POSITION;
    float4 vNormal          : NORMAL;
    float2 vTexcoord        : TEXCOORD0;
    float4 vWorldPos        : TEXCOORD1;
    float4 vProjPos         : TEXCOORD2;
    float4 vTangent         : TANGENT;
    float4 vBinormal        : BINORMAL;
};

struct VS_OUT_OUTLINE
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct VS_OUT_CASCADE_SHADOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN
{
    float4 vPosition        : SV_POSITION;
    float4 vNormal          : NORMAL;
    float2 vTexcoord        : TEXCOORD0;
    float4 vWorldPos        : TEXCOORD1;
    float4 vProjPos         : TEXCOORD2;
    float4 vTangent         : TANGENT;
    float4 vBinormal        : BINORMAL;
};

struct PS_IN_OUTLINE
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vDiffuse         : SV_TARGET0;
    float4 vNormal          : SV_TARGET1;
    float4 vDepth           : SV_TARGET2;
    float4 vORM             : SV_TARGET3;
    float4 vRimBloom        : SV_TARGET4; /* Rim + Bloom */
    float4 vEmissive        : SV_Target5;
};

struct PS_OUT_SHADOW
{
    vector vLightDepth : SV_TARGET0;
};

struct PS_OUT_OUTLINE
{
    vector vColor : SV_TARGET0;
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

    Out.vPosition   = mul(vPosition, matWVP);
    Out.vNormal     = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord   = In.vTexcoord;
    Out.vWorldPos   = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos    = Out.vPosition;
    Out.vTangent    = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal   = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));
	
    return Out;
}

VS_OUT_SHADOW VS_SHADOW_DEPTH(VS_IN In)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;
    
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
    Out.vProjPos = Out.vPosition;
	
    return Out;
}

VS_OUT_OUTLINE VS_MAIN_OUTLINE(VS_IN In)
{
    VS_OUT_OUTLINE Out = (VS_OUT_OUTLINE) 0;
    
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
    
    float4 OutPos = vector(vPosition.xyz + vNormal.xyz * g_LineThick, 1);
    Out.vPosition = mul(OutPos, matWVP);
    Out.vPosition.z -= 0.001f;
    Out.vTexcoord = In.vTexcoord;
	
    //Out.vPosition = vector(vPosition.xyz + vNormal.xyz * g_LineThick, 1);
    return Out;
}

VS_OUT_SHADOW VS_CASCADE_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;

    matrix matWVP;

    matWVP = mul(g_WorldMatrix, g_CascadeProj);

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		                g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		                g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		                g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    
    return Out;
}



/* ------------------- Base Pixel Shader (0) -------------------*/

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a == 0.f)
        discard;
    
    /* 0 ~ 1 */
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz;
    
	/* -1 ~ 1 */
    vPixelNormal = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal = mul(vPixelNormal, WorldMatrix);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    if(Out.vDiffuse.a < 0.1f)
        discard;
    
    /* ---------------- New ---------------- */
    // float4 vRimColor = Calculation_RimColor(In.vNormal, In.vPosition);
    // Out.vDiffuse += vRimColor;
    // Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) + vRimColor;
    
    return Out;
}

/* ------------------- Shadow Pixel Shader(2) -------------------*/

PS_OUT_SHADOW PS_MAIN_SHADOW(VS_OUT_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    Out.vLightDepth = In.vProjPos.w / g_fLightFar;
	
    return Out;
}

/* ------------------- Shadow Pixel Shader(3) -------------------*/
PS_OUT_SHADOW PS_CASCADE_SHADOW(VS_OUT_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    //Out.vLightDepth = (In.vProjPos.z, 0.f, 0.f, 0.f);
	
    Out.vLightDepth = In.vProjPos.z;
    
    return Out;
}
/* ------------------- Pixel Shader(4) -------------------*/
PS_OUT PS_BOSS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    if (vMtrlDiffuse.a < 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    /* ---------------- New ---------------- */
    float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    Out.vDiffuse += vRimColor;
    Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) + vRimColor;
    /* g_vBloomPower */

    // Out.vDiffuse += vRimColor; // 효과 약하게 하고싶으면 Bloom에 넣지말고 여기에 넣기 
    return Out;
}

/* ------------------- Pixel Shader(5) -------------------*/
PS_OUT PS_MAIN_RIMBLOOM_A(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    if (vMtrlDiffuse.a == 0.f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
 
    /* ---------------- New ---------------- */
    float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) + vRimColor;
    Out.vRimBloom += g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    // Out.vDiffuse += vRimColor; // 효과 약하게 하고싶으면 Bloom에 넣지말고 여기에 넣기 
    return Out;
}
/* ------------------- Pixel Shader(6) -------------------*/
PS_OUT PS_MAIN_RIMBLOOM_B(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    if (vMtrlDiffuse.a == 0.f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
 
    /* ---------------- New ---------------- */
    float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) ;
    
    
    // Out.vDiffuse += vRimColor; // 효과 약하게 하고싶으면 Bloom에 넣지말고 여기에 넣기 
    return Out;
}
/* ------------------- Pixel Shader(7) -------------------*/
PS_OUT PS_MAIN_RIMBLOOM_C(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    if (vMtrlDiffuse.a == 0.f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
 
    /* ---------------- New ---------------- */
    float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    Out.vDiffuse += vRimColor;
    Out.vRimBloom = Calculation_Brightness(Out.vDiffuse);
    
    
    // Out.vDiffuse += vRimColor; // 효과 약하게 하고싶으면 Bloom에 넣지말고 여기에 넣기 
    return Out;
}
/* ------------------- Pixel Shader(8) -------------------*/
PS_OUT PS_MAIN_RIMBLOOM_D(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a == 0.f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
 
    /* ---------------- New ---------------- */
    float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    Out.vDiffuse += vRimColor;
    Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) + vRimColor;
    
    
    // Out.vDiffuse += vRimColor; // 효과 약하게 하고싶으면 Bloom에 넣지말고 여기에 넣기 
    return Out;
}

/* ------------------- Pixel Shader(9) : HAIR  -------------------*/
PS_OUT PS_MAIN_HAIR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //float4 vDir = In.vPosition - g_vCamPosition;
    //float dotTL = dot(In.vTangent, g_vDirectionalDir);
    //float dotTV = dot(In.vTangent,)
    // sinTL = sqrt(1 - dotTL * dotTL);
    // sinTV = sqrt(1 - dotTV * dotTV);
    
    // calculate diffuse term
    // float3 diffuse = Kd * 
    
    /* --------------- */ 
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
 
    return Out;
}

/* ------------------- Pixel Shader(10) : OutLine -------------------*/
PS_OUT_OUTLINE PS_MAIN_OUTLINE(PS_IN_OUTLINE In)
{
    PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE) 0;
    
    float4 vColor = lerp(g_vLineColor, mul(g_vLineColor, float4(0.f, 0.f, 0.f, 0.f)), g_TimeDelta);
    
    Out.vColor = g_vLineColor;
    
    return Out;
}

/* ------------------- Pixel Shader(11) : RimBloom -------------------*/
PS_OUT PS_MAIN_PLAYERRIM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    return Out;
}

/* ------------------- Pixel Shader(12) : -------------------*/

/* ------------------- Pixel Shader(13) :  -------------------*/
/*=============================================================
 
                          Technique
                                
==============================================================*/
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

        VertexShader = compile vs_5_0 VS_SHADOW_DEPTH();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass Cascade // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CASCADE_SHADOW();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_CASCADE_SHADOW();

    }
    pass BossModel // 4
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

    pass RimBloom_A // 5
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

    pass RimBloom_B // 6
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
    pass RimBloom_C // 7
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
    pass RimBloom_D // 8
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

    pass Hair // 9
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HAIR();
    }

    pass OUTLINE
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_OUTLINE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
    }

    pass RIMBLOOM
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PLAYERRIM();
    }
}