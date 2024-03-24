#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4          g_vCamPosition;
float           g_fCamFar;
float           g_fLightFar;
float           g_fTimeDelta;

/* =========== Texture =========== */
Texture2D		g_DiffuseTexture;
Texture2D       g_NormalTexture;
Texture2D       g_SpecularTexture;
Texture2D       g_EmissiveTexture;
Texture2D       g_OpacityTexture;

Texture2D       g_ColorDiffuse;
Texture2D       g_MaskTexture;
Texture2D       g_NoiseTexture;

/* =========== Value =========== */
float g_fDissolveWeight;                        /* Dissolve  */

float4 g_vLineColor;                            /* OutLine */
float g_LineThick;                              /* OutLine */

float3 g_vBloomPower = { 0.f, 0.f, 0.f };       /* Bloom */
float4 g_vRimColor = { 0.f, 0.f, 0.f, 0.f };    /* RimLight */
float g_fRimPower = 5.f;                        /* RimLight */

/* ------------------- function ------------------- */ 
float2 RotateTexture(float2 texCoord, float angle)
{
    float2 rotatedTexCoord;
    rotatedTexCoord.x = texCoord.x * cos(angle) - texCoord.y * sin(angle);
    rotatedTexCoord.y = texCoord.x * sin(angle) + texCoord.y * cos(angle);
    
    return rotatedTexCoord;
}

cbuffer VS_CONSTANT_BUFFER
{
    matrix  mWorldViewProj;
    float4  vSomeVectorThatMayBeNeededByASpecificShader;
    float   fSomeFloatThatMayBeNeededByASpecificShader;
    float   fTime;
    float   fSomeFloatThatMayBeNeededByASpecificShader2;
    float   fSomeFloatThatMayBeNeededByASpecificShader3;
};

/* ------------------- ------------------- */ 
struct VS_IN
{
	float3		vPosition       : POSITION;
	float3		vNormal         : NORMAL;
	float2		vTexcoord       : TEXCOORD0;
	float3		vTangent        : TANGENT;
};

struct VS_OUT
{
	float4		vPosition       : SV_POSITION;
	float4		vNormal         : NORMAL;
	float2		vTexcoord       : TEXCOORD0;	
	float4		vWorldPos       : TEXCOORD1;
	float4		vProjPos        : TEXCOORD2;
	float4		vTangent        : TANGENT;
	float4		vBinormal       : BINORMAL;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
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
/* ------------------- Base Vertex Shader -------------------*/

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;
    
	matrix		matWV, matWVP;
    
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

	return Out;
}


/* ------------------- Base Pixel Shader (0) -------------------*/

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.0f)
        discard;
    
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal = mul(vPixelNormal, WorldMatrix);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
	return Out;
}

PS_OUT PS_MAIN_NORMAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.0f)
        discard;
    
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal = mul(vPixelNormal, WorldMatrix);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
	return Out;
}

/* ------------------- Skybox Pixel Shader(1) -------------------*/

PS_OUT PS_SKYBOX_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(1.f, 1.f, 0.f, 0.f);
    
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

    Out.vLightDepth = In.vProjPos.w / 500.f;
	
    return Out;
}
/* ------------------- Shadow Pixel Shader(3) -------------------*/
PS_OUT PS_MAIN_WHITE_BLINK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vColor;
    
    /* Normal Setting */ 
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    vColor = lerp(vMtrlDiffuse, mul(vMtrlDiffuse, float4(0.7f, 0.7f, 0.7f, 0.7f)), g_fTimeDelta);

    Out.vDiffuse = vColor;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    return Out;
}

/* ------------------- OutLine Pixel Shader(4) -------------------*/

VS_OUT VS_MAIN_OUTLINE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float4 OutPos = vector(In.vPosition.xyz + In.vNormal.xyz * g_LineThick, 1);
    Out.vPosition = mul(OutPos, matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

PS_OUT PS_MAIN_OUTLINE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vColor = { 1.f, 1.f, 1.f, 1.f };
    vColor.a = g_fTimeDelta;
    
    /* Normal Setting */ 
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vDiffuse = vColor;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    return Out;
}

PS_OUT PS_BloodPool(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fTime = g_fTimeDelta;
    float2 newUV = RotateTexture(In.vTexcoord, fTime);
   
    vector vMtrlDiffuse = g_ColorDiffuse.Sample(LinearSampler, newUV);
    vector vMtrlMask = g_MaskTexture.Sample(LinearSampler, newUV);
    vector vMtrlNoise = g_NoiseTexture.Sample(LinearSampler, newUV);
    
    if (vMtrlMask.r <= 0.6f)
        Out.vDiffuse = vector(0.275f, 1.f, 0.f, 1.f);
    else
        Out.vDiffuse = vector(1.f, 0.039f, 0.f, 1.f);
    
    //if (vMtrlDiffuse.a < 0.0f)
    //    discard;
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal = mul(vPixelNormal, WorldMatrix);
    
    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}

/* ------------------- Technique -------------------*/ 

technique11 DefaultTechnique
{	
	pass Model // 0번 패스
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

    pass Skybox // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SKYBOX_MAIN();
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

    pass White_Blink // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WHITE_BLINK();
    }

    pass OutLine // 4
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_OUTLINE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
    }

    pass Model_NoneCull_NoneDSS //5번 패스
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Model_BloodPool // 6번 패스 보스맵 피웅덩이 전용
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BloodPool();
    }

    pass Model_NormalMapping // 7번 패스
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
    }



    
}
