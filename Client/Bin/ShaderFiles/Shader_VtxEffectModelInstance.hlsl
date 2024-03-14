#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float       g_fTimeDelta;

Texture2D	g_DiffuseTexture;
Texture2D	g_MaskTexture;
Texture2D	g_NoiseTexture;

Texture2D	g_NormalTexture;
Texture2D   g_SpecularTexture;

// Camera ======================
vector      g_vCamPosition;
vector      g_vCamDirection;
float		g_fCamFar;
// =============================

/* UV */
float2      g_UVOffset;
float2      g_UVScale;
float		g_fDegree;


/* Color */
float4		g_vColor_Mul;


/* Discard */
float		g_fAlpha_Discard;
float3		g_vBlack_Discard;


/* Dissolve */
Texture2D	g_DissolveDiffTexture;
float		g_fDissolveWeight;
float		g_fDissolveRatio;



/* RimLight - 필요한거 두개뿐임(+1개 추가) */
float4		g_vRimColor;
float		g_fRimPower;
float3      g_vBloomPower;


// Distortion ====================
float	g_fFrameTime;
float3	g_vScrollSpeeds;
float3	g_vScales;

float2	g_vDistortion1;
float2	g_vDistortion2;
float2	g_vDistortion3;
float	g_fDistortionScale;
float	g_fDistortionBias;
// ================================


// Custom Function ==============================================================================================================
float2 Rotate_Texcoord(float2 vTexcoord, float fDegree)
{
	float fDegree2Radian = 3.14159265358979323846 * 2 / 360.f;
	float fRotationRadian = fDegree * fDegree2Radian;
	float cosA = cos(fRotationRadian);
	float sinA = sin(fRotationRadian);

	float2x2 RotateMatrix = float2x2(cosA, -sinA, sinA, cosA);

	vTexcoord -= 0.5f;
	vTexcoord = mul(vTexcoord, RotateMatrix);
	vTexcoord += 0.5f;

	return vTexcoord;
}


float2 RotateTexture(float2 texCoord, float angle)
{
    float2 rotatedTexCoord;
    rotatedTexCoord.x = texCoord.x * cos(angle) - texCoord.y * sin(angle);
    rotatedTexCoord.y = texCoord.x * sin(angle) + texCoord.y * cos(angle);
    
    return rotatedTexCoord;    
}



float IsIn_Range(float fMin, float fMax, float fValue)
{
	return (fMin <= fValue) && (fMax >= fValue);
}

float4 Calculation_RimColor(float4 In_Normal, float4 In_Pos)
{
    float fRimPower = 1.f - saturate(dot(In_Normal, normalize((-1.f * (In_Pos - g_vCamPosition)))));
    fRimPower = pow(fRimPower, 5.f) * g_fRimPower;
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
// Custom Function ==============================================================================================================



// MAIN =========================================================================================================================
struct VS_IN
{
	float3		vPosition	 : POSITION;
	float3		vNormal		 : NORMAL;
	float2		vTexUV		 : TEXCOORD0;
	float3		vTangent	 : TANGENT;

	float4		vRight		 : TEXCOORD1;
	float4		vUp			 : TEXCOORD2;
	float4		vLook		 : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;

};

struct VS_OUT
{
	float4		vPosition	: SV_POSITION;
	float4		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		vWorldPos	: TEXCOORD1;
	float4		vProjPos	: TEXCOORD2;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT) 0;

	
	matrix WorldMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector vPosition = mul(vector(In.vPosition, 1.f), WorldMatrix);

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}


struct PS_IN
{
	float4		vPosition	: SV_POSITION;
	float4		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		vWorldPos	: TEXCOORD1;
	float4		vProjPos	: TEXCOORD2;
};


struct PS_OUT
{	
	float4 vDiffuse			: SV_TARGET0;
    float4 vSolid			: SV_TARGET1;
	float4 vNormal			: SV_TARGET2;
	float4 vDepth			: SV_TARGET3;
	float4 vRimBloom		: SV_TARGET4;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT) 0;
	
    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);
	
	vector vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	float4 vAlphaColor = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	vDiffuseColor.a *= vAlphaColor;
	
	if (vDiffuseColor.a <= g_fAlpha_Discard)	// 알파 자르기
		discard;

	Out.vDiffuse = vDiffuseColor * g_vColor_Mul; // 색 곱하기

	
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
   

    /* ---------------- New ---------------- :  */
	float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
	Out.vDiffuse += vRimColor;
	//Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
	Out.vRimBloom = float4(g_vBloomPower, 1.0f);


	//// 검은색 잘라내기
	//if (Out.vDiffuse.r < g_vBlack_Discard.r && Out.vDiffuse.g < g_vBlack_Discard.g && Out.vDiffuse.b < g_vBlack_Discard.b)
	//	discard;

	return Out;

}
// MAIN =========================================================================================================================


// MAIN_SOLID ===================================================================================================================
PS_OUT PS_MAIN_SOLID(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);
	
    vector vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    float4 vAlphaColor = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

    vDiffuseColor.a *= vAlphaColor;
	
    if (vDiffuseColor.a <= g_fAlpha_Discard)	// 알파 자르기
        discard;

    Out.vDiffuse = vDiffuseColor * g_vColor_Mul; // 색 곱하기

	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
   

    /* ---------------- New ---------------- :  */
    float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    Out.vDiffuse += vRimColor;
	//Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
    Out.vRimBloom = float4(g_vBloomPower, 1.0f);

    Out.vSolid = Out.vDiffuse; 

    return Out;

}
// MAIN_SOLID ===================================================================================================================


//  Normal Mapping(MAIN_NORMAL) =================================================================================================
struct VS_OUT_NORMAL
{
	float4 vPosition	: SV_POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
	float4 vWorldPos	: TEXCOORD1;
	float4 vProjPos		: TEXCOORD2;
	float3 vTangent		: TANGENT;
	float3 vBinormal	: BINORMAL;
};

VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In)
{
	VS_OUT_NORMAL Out = (VS_OUT_NORMAL)0;


	matrix			WorldMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector			vPosition = mul(vector(In.vPosition, 1.f), WorldMatrix);

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));

	return Out;
}


struct PS_IN_NORMAL
{
	float4 vPosition	: SV_POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
	float4 vWorldPos	: TEXCOORD1;
	float4 vProjPos		: TEXCOORD2;
	float3 vTangent		: TANGENT;
	float3 vBinormal	: BINORMAL;
};


PS_OUT PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    clip(Out.vDiffuse.a - g_fAlpha_Discard);
    Out.vDiffuse.a = 1.f;
	/* 0 ~ 1 */
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;

	/* -1 ~ 1 */
	vPixelNormal = vPixelNormal * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

	vPixelNormal = mul(vPixelNormal, WorldMatrix);

	Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
	
	return Out;
}

//  Normal Mapping(MAIN_NORMAL) =================================================================================================



//  Dissolve ====================================================================================================================
PS_OUT PS_MAIN_Dissolve(PS_IN_NORMAL In)
{
	PS_OUT Out = (PS_OUT)0;

	vector TexDissolve = g_NoiseTexture.Sample(LinearSampler, In.vTexUV);

    clip(TexDissolve - g_fDissolveRatio);

    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);
	
    vector vTexDiff = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

    float fStepValue = IsIn_Range(0.f, 0.05f, TexDissolve.r - g_fDissolveRatio);
	

    Out.vDiffuse = (1.f - fStepValue) * vTexDiff + fStepValue * g_DissolveDiffTexture.Sample(LinearSampler, In.vTexUV);
	
    clip(Out.vDiffuse.a - g_fAlpha_Discard);
    //Out.vDiffuse.a = 1.f;

	float4 vAlphaMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a *= vAlphaMask.a;
	
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
	vPixelNormal = vPixelNormal * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

	vPixelNormal = mul(vPixelNormal, WorldMatrix);
	
	Out.vNormal    = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

	
	
	 /* ---------------- New ---------------- :  */
    float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    Out.vDiffuse += vRimColor;
	//Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
    Out.vRimBloom = float4(g_vBloomPower, 1.0f);

    Out.vSolid = Out.vDiffuse;
	
	
	
	return Out;
}
//  Dissolve ====================================================================================================================


//  DISTORTION ==================================================================================================================
struct VS_OUT_DISTORTION
{
	float4 vPosition	: SV_POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
	float4 vWorldPos	: TEXCOORD1;
	float4 vProjPos		: TEXCOORD2;
	float3 vTangent		: TANGENT;
	float3 vBinormal	: BINORMAL;

	float2 vTexcoord1	: TEXCOORD3;
	float2 vTexcoord2	: TEXCOORD4;
	float2 vTexcoord3	: TEXCOORD5;
};


VS_OUT_DISTORTION VS_MAIN_DISTORTION(VS_IN In)
{
	VS_OUT_DISTORTION Out = (VS_OUT_DISTORTION) 0;


	matrix WorldMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector vPosition = mul(vector(In.vPosition, 1.f), WorldMatrix);

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN_DISTORTION
{
	float4 vPosition	: SV_POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
	float4 vWorldPos	: TEXCOORD1;
	float4 vProjPos		: TEXCOORD2;
	float3 vTangent		: TANGENT;
	float3 vBinormal	: BINORMAL;
	
	float2 vTexcoord1	: TEXCOORD3;
	float2 vTexcoord2	: TEXCOORD4;
	float2 vTexcoord3	: TEXCOORD5;
};


// 디스토션 코드 리팩토링 좀 해야함...
PS_OUT PS_MAIN_DISTORTION(PS_IN_DISTORTION In)
{
	PS_OUT Out = (PS_OUT) 0;

	float4 vNoise1;
	float4 vNoise2;
	float4 vNoise3;

	float4	vFinalNoise;
	float	fPerturb;
	float2	vNoiseCoords;
	float4	vFinalColor;
	float4	vAlphaColor;

	// 노이즈 텍스쳐의 좌표를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산 x 3
	In.vTexcoord1 = (In.vTexUV * g_vScales.x);
	In.vTexcoord1.y = In.vTexcoord1.y + (g_fFrameTime * g_vScrollSpeeds.x);

	In.vTexcoord2 = (In.vTexUV * g_vScales.y);
	In.vTexcoord2.y = In.vTexcoord2.y + (g_fFrameTime * g_vScrollSpeeds.y);

	In.vTexcoord3 = (In.vTexUV * g_vScales.z);
	In.vTexcoord3.y = In.vTexcoord3.y + (g_fFrameTime * g_vScrollSpeeds.z);


	// 동일한 노이즈 텍스쳐를 서로 다른 세 텍스쳐 좌표를 사용하여 세 개의 다른 크기의 노이즈를 얻는다.
	vNoise1 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord1);
	vNoise2 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord2);
	vNoise3 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord3);

	
	// 노이즈 값의 범위를 (0, 1)에서 (-1, +1)이 되도록한다.
	vNoise1 = (vNoise1 - 0.5f) * 2.0f;
	vNoise2 = (vNoise2 - 0.5f) * 2.0f;
	vNoise3 = (vNoise3 - 0.5f) * 2.0f;

	// 노이즈의 x와 y값을 세 개의 다른 왜곡 x및 y좌표로 흩뜨린다.
	vNoise1.xy = vNoise1.xy * g_vDistortion1.xy;
	vNoise2.xy = vNoise2.xy * g_vDistortion2.xy;
	vNoise3.xy = vNoise3.xy * g_vDistortion3.xy;

	// 왜곡된 세 노이즈 값들을 하나의 노이즈로 함성한다.
	vFinalNoise = vNoise1 + vNoise2 + vNoise3;

	// 입력으로 들어온 텍스쳐의 Y좌표를 왜곡 크기와 바이어스 값으로 교란시킨다.
	// 이 교란은 텍스쳐의 위쪽으로 갈수록 강해져서 맨 위쪽에는 깜박이는 효과를 만들어낸다.
	fPerturb = ((1.0f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	// 불꽃 색상 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를 만든다.
	vNoiseCoords.xy = (vFinalNoise.xy * fPerturb) + In.vTexUV.xy;


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 불꽃 텍스쳐에서 색상을 샘플링한다.
	// clamp샘플러를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
	vFinalColor = g_DiffuseTexture.Sample(ClampSampler, vNoiseCoords.xy);


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 알파 텍스쳐에서 알파값을 샘플링한다. (불꽃의 투명도를 지정하는 데 사용)
	// clamp샘플러를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
	vAlphaColor = g_MaskTexture.Sample(ClampSampler, vNoiseCoords.xy);

	vFinalColor.a = vAlphaColor;

	Out.vDiffuse = vFinalColor;


	//////
	vector TexDissolve = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	clip(TexDissolve - g_fDissolveRatio);

	In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
	In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);

	vector vTexDiff = vFinalColor;

	float fStepValue = IsIn_Range(0.f, 0.05f, TexDissolve.r - g_fDissolveRatio);

	Out.vDiffuse = (1.f - fStepValue) * vTexDiff + fStepValue * g_DissolveDiffTexture.Sample(LinearSampler, In.vTexUV);

	clip(Out.vDiffuse.a - g_fAlpha_Discard);
	//Out.vDiffuse.a = 1.f;

	//float4 vAlphaMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	//Out.vDiffuse.a *= vAlphaMask.a;

	float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
	vPixelNormal = vPixelNormal * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

	vPixelNormal = mul(vPixelNormal, WorldMatrix);

	Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
	
	
    Out.vDiffuse *= g_vColor_Mul;
	
	/* ---------------- New ---------------- :  */
    float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    Out.vDiffuse += vRimColor;
	//Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
    Out.vRimBloom = float4(g_vBloomPower, 1.0f);
	
	
	return Out;
}
//  DISTORTION ==================================================================================================================



//  DISTORTION_SOLID ============================================================================================================
PS_OUT PS_MAIN_DISTORTION_SOLID(PS_IN_DISTORTION In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoise1;
    float4 vNoise2;
    float4 vNoise3;

    float4 vFinalNoise;
    float fPerturb;
    float2 vNoiseCoords;
    float4 vFinalColor;
    float4 vAlphaColor;

	// 노이즈 텍스쳐의 좌표를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산 x 3
    In.vTexcoord1 = (In.vTexUV * g_vScales.x);
    In.vTexcoord1.y = In.vTexcoord1.y + (g_fFrameTime * g_vScrollSpeeds.x);

    In.vTexcoord2 = (In.vTexUV * g_vScales.y);
    In.vTexcoord2.y = In.vTexcoord2.y + (g_fFrameTime * g_vScrollSpeeds.y);

    In.vTexcoord3 = (In.vTexUV * g_vScales.z);
    In.vTexcoord3.y = In.vTexcoord3.y + (g_fFrameTime * g_vScrollSpeeds.z);


	// 동일한 노이즈 텍스쳐를 서로 다른 세 텍스쳐 좌표를 사용하여 세 개의 다른 크기의 노이즈를 얻는다.
    vNoise1 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord1);
    vNoise2 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord2);
    vNoise3 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord3);

	
	// 노이즈 값의 범위를 (0, 1)에서 (-1, +1)이 되도록한다.
    vNoise1 = (vNoise1 - 0.5f) * 2.0f;
    vNoise2 = (vNoise2 - 0.5f) * 2.0f;
    vNoise3 = (vNoise3 - 0.5f) * 2.0f;

	// 노이즈의 x와 y값을 세 개의 다른 왜곡 x및 y좌표로 흩뜨린다.
    vNoise1.xy = vNoise1.xy * g_vDistortion1.xy;
    vNoise2.xy = vNoise2.xy * g_vDistortion2.xy;
    vNoise3.xy = vNoise3.xy * g_vDistortion3.xy;

	// 왜곡된 세 노이즈 값들을 하나의 노이즈로 함성한다.
    vFinalNoise = vNoise1 + vNoise2 + vNoise3;

	// 입력으로 들어온 텍스쳐의 Y좌표를 왜곡 크기와 바이어스 값으로 교란시킨다.
	// 이 교란은 텍스쳐의 위쪽으로 갈수록 강해져서 맨 위쪽에는 깜박이는 효과를 만들어낸다.
    fPerturb = ((1.0f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	// 불꽃 색상 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를 만든다.
    vNoiseCoords.xy = (vFinalNoise.xy * fPerturb) + In.vTexUV.xy;


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 불꽃 텍스쳐에서 색상을 샘플링한다.
	// clamp샘플러를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
    vFinalColor = g_DiffuseTexture.Sample(ClampSampler, vNoiseCoords.xy);


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 알파 텍스쳐에서 알파값을 샘플링한다. (불꽃의 투명도를 지정하는 데 사용)
	// clamp샘플러를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
    vAlphaColor = g_MaskTexture.Sample(ClampSampler, vNoiseCoords.xy);

    vFinalColor.a = vAlphaColor;

    Out.vDiffuse = vFinalColor;


	//////
    vector TexDissolve = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

    clip(TexDissolve - g_fDissolveRatio);

    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);

    vector vTexDiff = vFinalColor;

    float fStepValue = IsIn_Range(0.f, 0.05f, TexDissolve.r - g_fDissolveRatio);

    Out.vDiffuse = (1.f - fStepValue) * vTexDiff + fStepValue * g_DissolveDiffTexture.Sample(LinearSampler, In.vTexUV);

    clip(Out.vDiffuse.a - g_fAlpha_Discard);
	//Out.vDiffuse.a = 1.f;

	//float4 vAlphaMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	//Out.vDiffuse.a *= vAlphaMask.a;

    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vPixelNormal = mul(vPixelNormal, WorldMatrix);

    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
	
	
    Out.vDiffuse *= g_vColor_Mul;
	
	/* ---------------- New ---------------- :  */
    float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    Out.vDiffuse += vRimColor;
	//Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
    Out.vRimBloom = float4(g_vBloomPower, 1.0f);
	
	
    Out.vSolid = Out.vDiffuse;
	
    return Out;
}
//  DISTORTION_SOLID ============================================================================================================


//  DISTORTION_HORAIZON =========================================================================================================
PS_OUT PS_MAIN_DISTORTION_HORAIZON(PS_IN_DISTORTION In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoise1;
    float4 vNoise2;
    float4 vNoise3;

    float4 vFinalNoise;
    float fPerturb;
    float2 vNoiseCoords;
    float4 vFinalColor;
    float4 vAlphaColor;

	// 노이즈 텍스쳐의 좌표를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산 x 3
    In.vTexcoord1 = (In.vTexUV * g_vScales.x);
    In.vTexcoord1.x = In.vTexcoord1.x + (g_fFrameTime * g_vScrollSpeeds.x);

    In.vTexcoord2 = (In.vTexUV * g_vScales.y);
    In.vTexcoord2.x = In.vTexcoord2.x + (g_fFrameTime * g_vScrollSpeeds.y);

    In.vTexcoord3 = (In.vTexUV * g_vScales.z);
    In.vTexcoord3.x = In.vTexcoord3.x + (g_fFrameTime * g_vScrollSpeeds.z);


	// 동일한 노이즈 텍스쳐를 서로 다른 세 텍스쳐 좌표를 사용하여 세 개의 다른 크기의 노이즈를 얻는다.
    vNoise1 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord1);
    vNoise2 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord2);
    vNoise3 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord3);

	
	// 노이즈 값의 범위를 (0, 1)에서 (-1, +1)이 되도록한다.
    vNoise1 = (vNoise1 - 0.5f) * 2.0f;
    vNoise2 = (vNoise2 - 0.5f) * 2.0f;
    vNoise3 = (vNoise3 - 0.5f) * 2.0f;

	// 노이즈의 x와 y값을 세 개의 다른 왜곡 x및 y좌표로 흩뜨린다.
    vNoise1.xy = vNoise1.xy * g_vDistortion1.xy;
    vNoise2.xy = vNoise2.xy * g_vDistortion2.xy;
    vNoise3.xy = vNoise3.xy * g_vDistortion3.xy;

	// 왜곡된 세 노이즈 값들을 하나의 노이즈로 함성한다.
    vFinalNoise = vNoise1 + vNoise2 + vNoise3;

	// 입력으로 들어온 텍스쳐의 Y좌표를 왜곡 크기와 바이어스 값으로 교란시킨다.
	// 이 교란은 텍스쳐의 위쪽으로 갈수록 강해져서 맨 위쪽에는 깜박이는 효과를 만들어낸다.
    fPerturb = ((1.0f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	// 불꽃 색상 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를 만든다.
    vNoiseCoords.xy = (vFinalNoise.xy * fPerturb) + In.vTexUV.xy;


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 불꽃 텍스쳐에서 색상을 샘플링한다.
	// clamp샘플러를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
    vFinalColor = g_DiffuseTexture.Sample(ClampSampler, vNoiseCoords.xy);


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 알파 텍스쳐에서 알파값을 샘플링한다. (불꽃의 투명도를 지정하는 데 사용)
	// clamp샘플러를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
    vAlphaColor = g_MaskTexture.Sample(ClampSampler, vNoiseCoords.xy);

    vFinalColor.a = vAlphaColor;

    Out.vDiffuse = vFinalColor;


	//////
    vector TexDissolve = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

    clip(TexDissolve - g_fDissolveRatio);

    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);

    vector vTexDiff = vFinalColor;

    float fStepValue = IsIn_Range(0.f, 0.05f, TexDissolve.r - g_fDissolveRatio);

    Out.vDiffuse = (1.f - fStepValue) * vTexDiff + fStepValue * g_DissolveDiffTexture.Sample(LinearSampler, In.vTexUV);

    clip(Out.vDiffuse.a - g_fAlpha_Discard);
	//Out.vDiffuse.a = 1.f;

	//float4 vAlphaMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	//Out.vDiffuse.a *= vAlphaMask.a;

    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vPixelNormal = mul(vPixelNormal, WorldMatrix);

    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
	
	
    Out.vDiffuse *= g_vColor_Mul;
	
	/* ---------------- New ---------------- :  */
    float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    Out.vDiffuse += vRimColor;
	//Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
    Out.vRimBloom = float4(g_vBloomPower, 1.0f);
	

	
    return Out;
}
//  DISTORTION_HORAIZON =========================================================================================================


//  DISTORTION_HORAIZON_RIGHT ===================================================================================================
PS_OUT PS_MAIN_DISTORTION_HORAIZON_RIGHT(PS_IN_DISTORTION In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoise1;
    float4 vNoise2;
    float4 vNoise3;

    float4 vFinalNoise;
    float fPerturb;
    float2 vNoiseCoords;
    float4 vFinalColor;
    float4 vAlphaColor;

	// 노이즈 텍스쳐의 좌표를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산 x 3
    In.vTexcoord1 = (In.vTexUV * g_vScales.x);
    In.vTexcoord1.x = In.vTexcoord1.x + (g_fFrameTime * -g_vScrollSpeeds.x);

    In.vTexcoord2 = (In.vTexUV * g_vScales.y);
    In.vTexcoord2.x = In.vTexcoord2.x + (g_fFrameTime * -g_vScrollSpeeds.y);

    In.vTexcoord3 = (In.vTexUV * g_vScales.z);
    In.vTexcoord3.x = In.vTexcoord3.x + (g_fFrameTime * -g_vScrollSpeeds.z);


	// 동일한 노이즈 텍스쳐를 서로 다른 세 텍스쳐 좌표를 사용하여 세 개의 다른 크기의 노이즈를 얻는다.
    vNoise1 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord1);
    vNoise2 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord2);
    vNoise3 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord3);

	
	// 노이즈 값의 범위를 (0, 1)에서 (-1, +1)이 되도록한다.
    vNoise1 = (vNoise1 - 0.5f) * 2.0f;
    vNoise2 = (vNoise2 - 0.5f) * 2.0f;
    vNoise3 = (vNoise3 - 0.5f) * 2.0f;

	// 노이즈의 x와 y값을 세 개의 다른 왜곡 x및 y좌표로 흩뜨린다.
    vNoise1.xy = vNoise1.xy * g_vDistortion1.xy;
    vNoise2.xy = vNoise2.xy * g_vDistortion2.xy;
    vNoise3.xy = vNoise3.xy * g_vDistortion3.xy;

	// 왜곡된 세 노이즈 값들을 하나의 노이즈로 함성한다.
    vFinalNoise = vNoise1 + vNoise2 + vNoise3;

	// 입력으로 들어온 텍스쳐의 Y좌표를 왜곡 크기와 바이어스 값으로 교란시킨다.
	// 이 교란은 텍스쳐의 위쪽으로 갈수록 강해져서 맨 위쪽에는 깜박이는 효과를 만들어낸다.
    fPerturb = ((1.0f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	// 불꽃 색상 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를 만든다.
    vNoiseCoords.xy = (vFinalNoise.xy * fPerturb) + In.vTexUV.xy;


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 불꽃 텍스쳐에서 색상을 샘플링한다.
	// clamp샘플러를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
    vFinalColor = g_DiffuseTexture.Sample(ClampSampler, vNoiseCoords.xy);


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 알파 텍스쳐에서 알파값을 샘플링한다. (불꽃의 투명도를 지정하는 데 사용)
	// clamp샘플러를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
    vAlphaColor = g_MaskTexture.Sample(ClampSampler, vNoiseCoords.xy);

    vFinalColor.a = vAlphaColor;

    Out.vDiffuse = vFinalColor;


	//////
    vector TexDissolve = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

    clip(TexDissolve - g_fDissolveRatio);

    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);

    vector vTexDiff = vFinalColor;

    float fStepValue = IsIn_Range(0.f, 0.05f, TexDissolve.r - g_fDissolveRatio);

    Out.vDiffuse = (1.f - fStepValue) * vTexDiff + fStepValue * g_DissolveDiffTexture.Sample(LinearSampler, In.vTexUV);

    clip(Out.vDiffuse.a - g_fAlpha_Discard);
	//Out.vDiffuse.a = 1.f;

	//float4 vAlphaMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	//Out.vDiffuse.a *= vAlphaMask.a;

    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vPixelNormal = mul(vPixelNormal, WorldMatrix);

    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
	
	
    Out.vDiffuse *= g_vColor_Mul;
	
	/* ---------------- New ---------------- :  */
    float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    Out.vDiffuse += vRimColor;
	//Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
    Out.vRimBloom = float4(g_vBloomPower, 1.0f);
	

	
    return Out;
}
//  DISTORTION_HORAIZON_RIGHT ===================================================================================================


//  DISTORTION_ROTATION =========================================================================================================
PS_OUT PS_MAIN_DISTORTION_ROTATION(PS_IN_DISTORTION In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoise1;
    float4 vNoise2;
    float4 vNoise3;

    float4 vFinalNoise;
    float fPerturb;
    float2 vNoiseCoords;
    float4 vFinalColor;
    float4 vAlphaColor;

	// 노이즈 텍스쳐의 좌표를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산 x 3
    In.vTexcoord1 = (In.vTexUV * g_vScales.x);
    //In.vTexcoord1.x = In.vTexcoord1.x + (g_fFrameTime * g_vScrollSpeeds.x);
    In.vTexcoord1 = RotateTexture(In.vTexcoord1, g_fFrameTime * g_vScrollSpeeds.x);
	
	
    In.vTexcoord2 = (In.vTexUV * g_vScales.y);
    //In.vTexcoord2.x = In.vTexcoord2.x + (g_fFrameTime * g_vScrollSpeeds.y);
    In.vTexcoord2 = RotateTexture(In.vTexcoord2, g_fFrameTime * g_vScrollSpeeds.y);
	

    In.vTexcoord3 = (In.vTexUV * g_vScales.z);
   // In.vTexcoord3.x = In.vTexcoord3.x + (g_fFrameTime * g_vScrollSpeeds.z);
    In.vTexcoord3 = RotateTexture(In.vTexcoord3, g_fFrameTime * g_vScrollSpeeds.z);

	
	// 동일한 노이즈 텍스쳐를 서로 다른 세 텍스쳐 좌표를 사용하여 세 개의 다른 크기의 노이즈를 얻는다.
    vNoise1 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord1);
    vNoise2 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord2);
    vNoise3 = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord3);

	
	// 노이즈 값의 범위를 (0, 1)에서 (-1, +1)이 되도록한다.
    vNoise1 = (vNoise1 - 0.5f) * 2.0f;
    vNoise2 = (vNoise2 - 0.5f) * 2.0f;
    vNoise3 = (vNoise3 - 0.5f) * 2.0f;

	// 노이즈의 x와 y값을 세 개의 다른 왜곡 x및 y좌표로 흩뜨린다.
    vNoise1.xy = vNoise1.xy * g_vDistortion1.xy;
    vNoise2.xy = vNoise2.xy * g_vDistortion2.xy;
    vNoise3.xy = vNoise3.xy * g_vDistortion3.xy;

	// 왜곡된 세 노이즈 값들을 하나의 노이즈로 함성한다.
    vFinalNoise = vNoise1 + vNoise2 + vNoise3;

	// 입력으로 들어온 텍스쳐의 Y좌표를 왜곡 크기와 바이어스 값으로 교란시킨다.
	// 이 교란은 텍스쳐의 위쪽으로 갈수록 강해져서 맨 위쪽에는 깜박이는 효과를 만들어낸다.
    fPerturb = ((1.0f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	
	// 불꽃 색상 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를 만든다.
    vNoiseCoords.xy = (vFinalNoise.xy * fPerturb) + In.vTexUV.xy;


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 불꽃 텍스쳐에서 색상을 샘플링한다.
	// clamp샘플러를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
    vFinalColor = g_DiffuseTexture.Sample(ClampSampler, vNoiseCoords.xy);


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 알파 텍스쳐에서 알파값을 샘플링한다. (불꽃의 투명도를 지정하는 데 사용)
	// clamp샘플러를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
    vAlphaColor = g_MaskTexture.Sample(ClampSampler, vNoiseCoords.xy);

    vFinalColor.a = vAlphaColor;

    Out.vDiffuse = vFinalColor;


	//////
    vector TexDissolve = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

    clip(TexDissolve - g_fDissolveRatio);

    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);

    vector vTexDiff = vFinalColor;

    float fStepValue = IsIn_Range(0.f, 0.05f, TexDissolve.r - g_fDissolveRatio);

    Out.vDiffuse = (1.f - fStepValue) * vTexDiff + fStepValue * g_DissolveDiffTexture.Sample(LinearSampler, In.vTexUV);

    clip(Out.vDiffuse.a - g_fAlpha_Discard);
	//Out.vDiffuse.a = 1.f;

	//float4 vAlphaMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);
	//Out.vDiffuse.a *= vAlphaMask.a;

    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vPixelNormal = mul(vPixelNormal, WorldMatrix);

    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
	
	
    Out.vDiffuse *= g_vColor_Mul;
	
	/* ---------------- New ---------------- :  */
    float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    Out.vDiffuse += vRimColor;
	//Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
    Out.vRimBloom = float4(g_vBloomPower, 1.0f);
	

	
    return Out;
}
//  DISTORTION_ROTATION =========================================================================================================



technique11 DefaultTechnique
{
    pass Effect // 0
    {	
		//SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff); //SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);	
		SetRasterizerState(RS_Cull_None); //SetRasterizerState(RS_Default);

		VertexShader	= compile vs_5_0 VS_MAIN();
		HullShader		= NULL;
		DomainShader	= NULL;
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

    pass Effect_Solid // 1
    {
		//SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff); //SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None); //SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SOLID();
    }

	pass WireFrame // 2
	{
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_NoneCull_Wireframe);

		VertexShader	= compile vs_5_0 VS_MAIN();
		HullShader		= NULL;
		DomainShader	= NULL;
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

    pass Dissolve // 3
    {
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader	= compile vs_5_0 VS_MAIN_NORMAL();
        HullShader		= NULL;
        DomainShader	= NULL;
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_MAIN_Dissolve();
    }
	
	pass Distortion // 4
	{
		SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthStencilEnable, 0);
		SetRasterizerState(RS_Cull_None);

		VertexShader	= compile vs_5_0 VS_MAIN_DISTORTION();
		HullShader		= NULL;
		DomainShader	= NULL;
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN_DISTORTION();
	}

    pass Distortion_Solid // 5
    {
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_SOLID();
    }

    pass Distortion_Horaizon // 6
    {
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_HORAIZON();
    }

    pass Distortion_Horaizon_Right // 7
    {
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_HORAIZON_RIGHT();
    }


    pass Distortion_Rotation // 8
    {
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_ROTATION();
    }

}