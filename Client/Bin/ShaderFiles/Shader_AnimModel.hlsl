#include "Shader_Defines.hlsli"

/* Base */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_BoneMatrices[800];
float			g_fCamFar;
float           g_TimeDelta;

texture2D       g_DiffuseTexture;
texture2D       g_NormalTexture;
texture2D       g_SpecularTexture;

/* Dissolve  */
Texture2D		g_DissolveTexture;
float			g_fDissolveWeight;

/* Masking */
Texture2D		g_MaskingTexture;

/* Bloom */
float4          g_BloomColor;

/* Reflection */
matrix			g_ReflectionMatrix;

/* OutLine */
float4          g_vLineColor;
float           g_LineThick;

/* RimLight */
float4			g_vRimColor = { 0.f, 0.f, 0.f, 0.f }; 
float4          g_vCamPosition;

struct VS_IN
{
	float3		vPosition		: POSITION;
	float3		vNormal			: NORMAL;
	float2		vTexcoord		: TEXCOORD0;
	float3		vTangent		: TANGENT;
	uint4		vBlendIndices	: BLENDINDEX;
	float4		vBlendWeights	: BLENDWEIGHT;
};


struct VS_OUT
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal			: NORMAL;
	float2		vTexcoord		: TEXCOORD0;	
	float4		vWorldPos		: TEXCOORD1;
	float4		vProjPos		: TEXCOORD2;
	
    float3		vViewNormal		: NORMAL1;		/* ssao */ 
    float3		vPositionView	: POSITION;
};

/* ------------------- Base Vertex Shader -------------------*/
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		                     g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		                     g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		                     g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	
	// SSAO
    Out.vViewNormal = mul(Out.vNormal.xyz, (float3x3) g_ViewMatrix);
    Out.vPositionView = mul(float4(In.vPosition, 1.0f), matWV);
	
	return Out;
}

struct PS_IN
{
	float4	vPosition		: SV_POSITION;
	float4	vNormal			: NORMAL;
	float2	vTexcoord		: TEXCOORD0;
	float4	vWorldPos		: TEXCOORD1;
    float4	vProjPos		: TEXCOORD2;
    
	/* ssao */
    float3	vViewNormal		: NORMAL1;
    float3	vPositionView	: POSITION;
};

struct PS_OUT 
{
	float4	vDiffuse        : SV_TARGET0;
	float4	vNormal         : SV_TARGET1;
    float4  vDepth          : SV_TARGET2;
    float4  vORM            : SV_TARGET3;
    float4  vViewNormal     : SV_TARGET4;
    float4  vBloom          : SV_TARGET5;
};

/* ------------------- Base Pixel Shader (0) -------------------*/

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if (vMtrlDiffuse.a < 0.1f)
		discard;
    
    /* ¸²¶óÀÌÆ® -> ÇÁ·¹³Ú °ø½Ä »ç¿ë */
    
    float fRimPower = 1.f - saturate(dot(In.vNormal, normalize((-1.f * (In.vWorldPos - g_vCamPosition)))));
    fRimPower = pow(fRimPower, 5.f);
    vector vRimColor = g_vRimColor * fRimPower;
    
    Out.vDiffuse = vMtrlDiffuse + vRimColor;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vViewNormal = float4(normalize(In.vViewNormal), In.vPositionView.z);
	return Out;
}

/* ------------------- Shadow Pixel Shader(2) -------------------*/

struct PS_OUT_SHADOW
{
	vector		vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
	PS_OUT_SHADOW		Out = (PS_OUT_SHADOW)0;

	Out.vLightDepth = In.vProjPos.w / g_fCamFar;
	
	return Out;
}

/* ------------------- Dissolve Pixel Shader(3) -------------------*/

PS_OUT PS_MAIN_DISSOVLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord);
    float4 vDissolve = g_DissolveTexture.Sample(ClampSampler, In.vTexcoord);

    float sinTime = sin(g_fDissolveWeight);
    
    if (vColor.a == 0.f)
        clip(-1);

    if (vDissolve.r >= sinTime)
        vColor.a = 1;
    else
        vColor.a = 0;

    if (vDissolve.r >= sinTime - 0.05 && vDissolve.r <= sinTime + 0.05)
        vColor = float4(1, 0, 0, 1); // »¡
    else;

    if (vDissolve.r >= sinTime - 0.03 && vDissolve.r <= sinTime + 0.03)
        vColor = float4(1, 1, 0, 1); // ³ë
    else;

    if (vDissolve.r >= sinTime - 0.02 && vDissolve.r <= sinTime + 0.02)
        vColor = float4(1, 1, 1, 1); // Èò
    else;
	
    if (vDissolve.r >= sinTime - 0.005 && vDissolve.r <= sinTime + 0.005)
        vColor = g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord); // Èò
    else;
    
    if (0 == Out.vDiffuse.a)
        discard;
    
    Out.vDiffuse = vColor;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vViewNormal = float4(1.0f, 1.0f, 1.0f, 1.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

/* ------------------- Distortion Pixel Shader(4) -------------------*/

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    return Out;
}

/* ------------------- GrayScale Pixel Shader(5) -------------------*/

PS_OUT PS_MAIN_GRAY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
  
    float4 vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    float grayscale = (vColor.r * 0.3 + vColor.g * 0.59 + vColor.b * 0.11);

    if (vColor.a < 0.1f)
        discard;
    
    Out.vDiffuse = float4(grayscale, grayscale, grayscale, vColor.a);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vViewNormal = float4(1.0f, 1.0f, 1.0f, 1.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}

/* ------------------- Masking Pixel Shader(6) -------------------*/

PS_OUT PS_MAIN_MASKING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vMaskingTexture = g_MaskingTexture.Sample(LinearSampler, In.vTexcoord);
	
    float4 vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);

    vColor.a = vMaskingTexture.g;
	
    if (vColor.a < 0.5)
        discard;
    
    Out.vDiffuse = vColor;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vViewNormal = Out.vNormal;
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
	
    return Out;
}

/* ------------------- Bloom Pixel Shader(7) -------------------*/

PS_OUT PS_MAIN_BLOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vBloom = g_BloomColor;
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vViewNormal = Out.vNormal;
    
    return Out;
}
/* ------------------- Outline Pixel Shader(8) -------------------*/

struct VS_OUT_OUTLINE
{
    float4 vPosition    : SV_POSITION;
    float4 vNormal      : NORMAL;
    
    float2 vTexcoord    : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
    float4 vTangent     : TANGENT;
    float4 vBinormal    : BINORMAL;
    
    float3 vViewNormal  : NORMAL1; /* ssao */
    float3 vPositionView : POSITION;
};

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
    Out.vTexcoord = In.vTexcoord;
    
	// SSAO
    Out.vViewNormal = mul(Out.vNormal.xyz, (float3x3) g_ViewMatrix);
    Out.vPositionView = mul(float4(In.vPosition, 1.0f), matWV);
    
    return Out;
}

struct PS_IN_OUTLINE
{
    float4 vPosition        : SV_POSITION;
    float4 vNormal          : NORMAL;
    float2 vTexcoord        : TEXCOORD0;
    float4 vWorldPos        : TEXCOORD1;
    float4 vProjPos         : TEXCOORD2;
    float4 vTangent         : TANGENT;
    float4 vBinormal        : BINORMAL;
    float3 vViewNormal      : NORMAL1; /* ssao */
    float3 vPositionView    : POSITION;
};

PS_OUT PS_MAIN_OUTLINE(PS_IN_OUTLINE In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vColor = { 1.f, 1.f, 1.f, 1.f };
    vColor.a = g_TimeDelta;
    
    /* Normal Setting */ 
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vPosition.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vDiffuse    = vColor;
    Out.vNormal     = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth      = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    
    Out.vORM        = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vViewNormal = float4(normalize(In.vViewNormal), In.vPositionView.z);
    
    return Out;
}

/* ------------------- Technique -------------------*/ 
technique11 DefaultTechnique
{	
	pass Model // 0
	{			
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Model_Wireframe // 1
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

    pass Dissolve // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOVLE();
    }

    pass Distortion // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
    }

    pass GrayScale // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GRAY();

    }

    pass Masking // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MASKING();
    }

    pass Bloom // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM();
    }

    pass Out_Line // 8
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
    //pass Motion_Trail // 8
    //{
    //
    //}
}
