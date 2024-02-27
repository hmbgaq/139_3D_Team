#include "Shader_Defines.hlsli"

/* Base */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_BoneMatrices[800];
float			g_fCamFar;
float           g_TimeDelta;

Texture2D       g_DiffuseTexture;
Texture2D       g_NormalTexture;
Texture2D       g_SpecularTexture;

/* Dissolve  */
Texture2D		g_DissolveTexture;
float			g_fDissolveWeight;

/* Masking */
Texture2D		g_MaskingTexture;

/* Bloom */
float4          g_BloomColor = { 0.f, 0.f, 0.f, 0.f };
float3          g_vBloomPower;

/* Reflection */
matrix			g_ReflectionMatrix;

/* OutLine */
float4          g_vLineColor;
float           g_LineThick;

/* RimLight */
float4			g_vRimColor = { 0.f, 0.f, 0.f, 0.f }; 
float4          g_vCamPosition;

/* cascade */
matrix CascadeViewProj;
matrix g_CascadeProj;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};

/* -------------------------------------- */

struct KeyframeDesc
{
    int iAnimIndex;
    uint iCurFrame;
    uint iNextFrame;
    float fRatio;
    float fFrameAcc;

    uint iFinish;
    uint iFix;
    uint iStop;
};

struct TweenFrameDesc
{
    KeyframeDesc cur;
    KeyframeDesc next;

    float fTweenDuration;
    float fTweenRatio;
    float fTweenAcc;
    float fPadding;
};

TweenFrameDesc g_TweenFrames;
Texture2DArray g_TransformMap;

matrix Create_BoneMatrix_By_Lerp(VS_IN In)
{
    float indices[4] = { In.vBlendIndices.x, In.vBlendIndices.y, In.vBlendIndices.z, In.vBlendIndices.w };
    float weights[4] = { In.vBlendWeights.x, In.vBlendWeights.y, In.vBlendWeights.z, In.vBlendWeights.w };

    int animIndex[2];
    int currFrame[2];
    int nextFrame[2];
    float ratio[2];

	/* cur */
    animIndex[0] = g_TweenFrames.cur.iAnimIndex;
    currFrame[0] = g_TweenFrames.cur.iCurFrame;
    nextFrame[0] = g_TweenFrames.cur.iNextFrame;
    ratio[0] = g_TweenFrames.cur.fRatio;

	/* next */
    animIndex[1] = g_TweenFrames.next.iAnimIndex;
    currFrame[1] = g_TweenFrames.next.iCurFrame;
    nextFrame[1] = g_TweenFrames.next.iNextFrame;
    ratio[1] = g_TweenFrames.next.fRatio;

    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    float4x4 curr = 0;
    float4x4 next = 0;
    
    float4x4 transform = 0;

    for (int i = 0; i < 4; i++)
    {
		/* cur */
        c0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0)) * weights[i];
        c1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0)) * weights[i];
        c2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0)) * weights[i];
        c3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], animIndex[0], 0)) * weights[i];
        curr = float4x4(c0, c1, c2, c3);

        n0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0)) * weights[i];
        n1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0)) * weights[i];
        n2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0)) * weights[i];
        n3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0)) * weights[i];
        next = float4x4(n0, n1, n2, n3);

        float4x4 result = lerp(curr, next, ratio[0]);
		
		/* if next */
        if (0 <= animIndex[1])
        {
            c0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0)) * weights[i];
            c1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0)) * weights[i];
            c2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0)) * weights[i];
            c3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0)) * weights[i];
            curr = float4x4(c0, c1, c2, c3);

            n0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0)) * weights[i];
            n1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0)) * weights[i];
            n2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0)) * weights[i];
            n3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0)) * weights[i];
            next = float4x4(n0, n1, n2, n3);
            
            // Origin 
            {
                matrix nextResult = lerp(curr, next, ratio[1]);
                         
                result = lerp(result, nextResult, g_TweenFrames.fTweenRatio);
            }
            
        }
        transform += result;
    }

    return transform;
}

float4 Caculation_Brightness(float4 vColor, float3 BloomPower)
{
    float4 vBrightnessColor = float4(0.f, 0.f, 0.f, 0.f);

    float fPixelBrightness = dot(vColor.rgb, BloomPower.rgb);
    
    if (fPixelBrightness > 0.99f)
        vBrightnessColor = float4(vColor.rgb, 1.0f);

    return vBrightnessColor;
}

/* ------------------- ------------------- */ 


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
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    
    /* 림라이트 -> 프레넬 공식 사용 */
    /* 노말 (내적) 정점이 카메라를 바라보는방향 -> 카메라가 조명처럼 인식된다. */ 
    //float fRim = saturate(dot(In.vNormal, (g_vCamPosition - In.vWorldPos)));
    //int iRimPower = 5.f;
    ///* 일정이상보다 작으면 Rim을 없앤다. */
    //if(fRim > 0.3)
    //    fRim = 1;
    //else
    //    fRim = -1;
    
    //Out.vEmissive = Out.vORM + float4(pow(1 - fRim, iRimPower) * g_vRimColor.xyz, 1.f);
    
    float fRimPower = 1.f - saturate(dot(In.vNormal, normalize((-1.f * (In.vWorldPos - g_vCamPosition)))));
    fRimPower = pow(fRimPower, 5.f);
    vector vRimColor = g_vRimColor * fRimPower;
    
    Out.vBloom = Caculation_Brightness(Out.vDiffuse, g_vBloomPower) + vRimColor;
    Out.vDiffuse += vRimColor;
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
        vColor = float4(1, 0, 0, 1); // 빨
    else;

    if (vDissolve.r >= sinTime - 0.03 && vDissolve.r <= sinTime + 0.03)
        vColor = float4(1, 1, 0, 1); // 노
    else;

    if (vDissolve.r >= sinTime - 0.02 && vDissolve.r <= sinTime + 0.02)
        vColor = float4(1, 1, 1, 1); // 흰
    else;
	
    if (vDissolve.r >= sinTime - 0.005 && vDissolve.r <= sinTime + 0.005)
        vColor = g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord); // 흰
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

/* ------------------- Cascade Pixel Shader(9) -------------------*/
struct VS_CASCADE_OUT
{
    float4 vPosition : SV_POSITION;
};

VS_CASCADE_OUT VS_MAIN_CASCADE(VS_IN In)
{
    VS_CASCADE_OUT Out = (VS_CASCADE_OUT) 0;
    //float4x4 BoneMatrix = Create_BoneMatrix_By_Lerp(In);
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		                     g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		                     g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		                     g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    
    return Out;

}

struct GS_IN
{
    float4 vPosition : SV_POSITION;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(9)]
void GS_CASCADE(triangle GS_IN input[3], inout TriangleStream<GS_OUT> output)
{
    for (int face = 0; face < 3; ++face)
    {
        GS_OUT element = (GS_OUT) 0;
        element.RTIndex = face;
        
        for (int i = 0; i < 3; ++i)
        {
            element.vPosition = mul(input[i].vPosition, CascadeViewProj[face]);
            output.Append(element);
        }
        
        output.RestartStrip();
    }
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
  
    pass Cascade_Shadow // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_CASCADE();
        GeometryShader = compile gs_5_0 GS_CASCADE();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = NULL;
    }
}
