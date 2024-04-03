#include "Shader_Defines.hlsli"

/*======================== 예 시 파 일 =======================
// 1. 디졸브 - Bandit Sniper - Body 
==============================================================*/

/*=============================================================
 
                             Value
                                
==============================================================*/
/* =========== Common =========== */
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix      g_BoneMatrices[800];
float4      g_vCamPosition;
float       g_fCamFar;
float       g_fLightFar;
float       g_TimeDelta;

float4      g_CheckColor = { 1.f, 0.f, 0.f, 1.f };
bool        g_bORM_Available;
bool        g_bEmissive_Available;

/* =========== Texture =========== */
Texture2D   g_DiffuseTexture;       /* Noblend */
Texture2D   g_NormalTexture;        /* Noblend */
Texture2D   g_SpecularTexture;      /* Noblend */
Texture2D   g_EmissiveTexture;      /* Noblend */
Texture2D   g_OpacityTexture;       /* Noblend */

Texture2D   g_DistortionTexture;
Texture2D   g_DissolveTexture;
Texture2D   g_AlphaTexture;
Texture2D   g_MaskingTexture;
Texture2D   g_NoiseTexture;

/* =========== Value =========== */
float       g_Dissolve_Weight = 0.f;                /* Dissolve - 디졸브 가중치  */
float       g_Dissolve_feather = 0.1;               /* Dissolve - 마스크의 테두리를 부드럽게 만드는 데 사용*/
float3      g_Dissolve_Color = { 0.f, 0.f, 0.f };   /* Dissolve - 디졸브 사라지기 직전에 보이는 색상 */
float       g_Dissolve_ColorRange = 0.1f;           /* 위의 직전 보이는 색상이 어디까지 보일것인지 */ 

float4      g_vLineColor;                           /* OutLine */
float       g_LineThick;                            /* OutLine */

float3      g_vBloomPower = { 0.f, 0.f, 0.f };      /* Bloom */
float4      g_vRimColor   = { 0.f, 0.f, 0.f, 0.f }; /* RimLight */
float       g_fRimPower   = 5.f;                    /* RimLight */

matrix      g_CascadeProj;                          /* Cascade */

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
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition    : SV_POSITION;
    float4 vNormal      : NORMAL;
    float2 vTexcoord    : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
}; 

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct VS_OUT_OUTLINE
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_CASCADE_SHADOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN
{
    float4 vPosition    : SV_POSITION;
    float4 vNormal      : NORMAL;
    float2 vTexcoord    : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
};

struct PS_IN_OUTLINE
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vDiffuse     : SV_TARGET0;
    float4 vNormal      : SV_TARGET1;
    float4 vDepth       : SV_TARGET2;
    float4 vORM         : SV_TARGET3;
    float4 vRimBloom    : SV_TARGET4; /* Rim + Bloom */
    float4 vEmissive    : SV_TARGET5;
};

struct PS_OUT_SHADOW
{
    vector vLightDepth : SV_TARGET0;
};

struct PS_OUT_OUTLINE
{
    vector vColor : SV_TARGET0;
};

/*=============================================================
 
                        Vertex Shader
                                
==============================================================*/

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

/*=============================================================
 
                        Pixel Shader
                                
==============================================================*/

/* ------------------- Pixel Shader(0) : 노말셰이더  -------------------*/
PS_OUT PS_MAIN(PS_IN In)
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
    
    Out.vORM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
            
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    return Out;
}

/* ------------------- Pixel Shader(1) : 후면추리기x, 블랜드상태 등 -> 추가할거 추가할수있음   -------------------*/

/* ------------------- Pixel Shader(2) : 가상의 빛의 위치에서 그림자 깊이 기록  -------------------*/

PS_OUT_SHADOW PS_MAIN_SHADOW(VS_OUT_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    //Out.vLightDepth = float4(In.vProjPos.z / In.vProjPos.w, 0.0f, 0.0f, 0.0f);
    
    Out.vLightDepth = In.vProjPos.w / g_fLightFar;
    
    return Out;
}

/* ------------------- Pixel Shader(3) : Cascade Shadow -------------------*/
PS_OUT_SHADOW PS_CASCADE_SHADOW(VS_OUT_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    //Out.vLightDepth = (In.vProjPos.z, 0.f, 0.f, 0.f);
	
    Out.vLightDepth = In.vProjPos.z;
    
    return Out;
}

/* ------------------- Pixel Shader(4) : Dissolve -------------------*/
PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4  MtrlColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4  DissolveTexture = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
    float   fDissolve = DissolveTexture.r;
  
    float adjustedMask = fDissolve * (1 - g_Dissolve_feather) + g_Dissolve_feather / 2;
    float alpha = saturate((adjustedMask - g_Dissolve_Weight) / g_Dissolve_feather + 0.5);
    float4 FinalColor = MtrlColor * alpha;
    
    if (FinalColor.a == 0)
        discard;
    
    if (FinalColor.a > 0.f && FinalColor.a <= g_Dissolve_ColorRange)
        FinalColor.rgb = g_Dissolve_Color;
    
    Out.vDiffuse = FinalColor;
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    if (true == g_bORM_Available)
        Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (true == g_bEmissive_Available)
        Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}
/* ------------------- Pixel Shader(5) : MeshCheck -------------------*/
PS_OUT PS_MAIN_CHECK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_CheckColor;
    
    return Out;
}

/* ------------------- Pixel Shader(7) : OutLine Blink  -------------------*/
PS_OUT_OUTLINE PS_MAIN_OUTLINE(PS_IN_OUTLINE In)
{
    PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE) 0;
    
    float4 vColor = lerp(g_vLineColor, mul(g_vLineColor, float4(0.f, 0.f, 0.f, 0.f)), g_TimeDelta);
    
    //vector vColor = g_vLineColor;
    //vColor.a = g_TimeDelta;
    
    Out.vColor = vColor;
    
    return Out;
}
/* ------------------- Pixel Shader(8) : OutLine : Keep Color  -------------------*/
PS_OUT_OUTLINE PS_MAIN_OUTLINE_Keep(PS_IN_OUTLINE In)
{
    PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE) 0;
    
    vector vColor = g_vLineColor;
    
    Out.vColor = vColor;
    
    return Out;
}

/* ------------------- Pixel Shader() : Infected -------------------*/
PS_OUT PS_INFECTED_WEAPON(PS_IN In)
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
    Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) + vRimColor;
    
    return Out;
}

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

    pass Dissolve // 4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
    }

    pass MeshCheck // 5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CHECK();
    }

    pass OutLine_Blink // 6
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

    pass OutLine_Keep // 7
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_OUTLINE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTLINE_Keep();
    }

    pass Infected_Weapon // 8
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_INFECTED_WEAPON();
    }

}