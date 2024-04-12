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

bool        g_bUV_Wave;
float2      g_UV_WaveSpeed;
float2      g_UVOffset_Mask;
float2      g_UVScale_Mask;


/* Color */
int			g_iColorMode;
float4		g_vColor_Mul;


/* Discard */
float		g_fAlpha_Discard;
float3		g_vBlack_Discard;


/* Dissolve */
float		g_fDissolveRatio;


/* RimLight - 필요한거 두개뿐임(+1개 추가) */
float4		g_vRimColor;
float		g_fRimPower;
float3      g_vBloomPower;


// Distortion ====================
float	g_fFrameTime;

int     g_iScrollType;

float3	g_vScrollSpeeds;
float3	g_vScales;

float2	g_vDistortion1;
float2	g_vDistortion2;
float2	g_vDistortion3;
float	g_fDistortionScale;
float	g_fDistortionBias;
// ================================


struct EffectDesc
{
    // 16 배수여야함 (64)
   
    float4 g_vColors_Mul; // 16
    
    float3 g_vRight; // 12
    float  g_fCurAddAlpha; // 4
    
    float3 g_vUp; // 12
    float g_fPadding2; // 4
    
    float3 g_vLook; // 12
    float g_fPadding3; // 4
};
EffectDesc g_EffectDesc[1000];



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


float4 Calculation_ColorBlend(float4 vDiffuse, float4 vBlendColor, int iColorMode)
{
    float4 vResault = vDiffuse;
    if (0 == iColorMode)
    {
      // 곱하기
        vResault = vResault * vBlendColor;
    }
    else if (1 == iColorMode)
    {
      // 스크린
        vResault = 1.f - ((1.f - vResault) * (1.f - vBlendColor));
    }
    else if (2 == iColorMode)
    {
      // 오버레이
        vResault = max(vResault, vBlendColor);
    }
    else if (3 == iColorMode)
    {
      // 더하기
        vResault = vResault + vBlendColor;
    }
    else if (4 == iColorMode)
    {
      // 번(Burn)
        vResault = vResault + vBlendColor - 1.f;
    }
    else if (5 == iColorMode)
    {
        // 비비드 라이트
        for (int i = 0; i < 3; ++i)
        {
            vResault[i] = (vBlendColor[i] < 0.5f) ? (1.f - (1.f - vDiffuse[i]) / (2.f * vBlendColor[i]))
            : (vDiffuse[i] / (2.f * (1.f - vBlendColor[i])));

        }
        
        vResault.a = vDiffuse.a;
    }
    else if (6 == iColorMode)
    {
        // 소프트 라이트
        for (int i = 0; i < 3; ++i)
        {
            if (vBlendColor[i] < 0.5f)
            {
                vResault[i] = 2.f * vDiffuse[i] * vBlendColor[i] +
                    vDiffuse[i] * vDiffuse[i] * (1.f - 2.f * vBlendColor[i]);
            }
            else
            {
                vResault[i] = 2.f * vDiffuse[i] * (1.f - vBlendColor[i]) +
                    sqrt(vDiffuse[i]) * (2.f * vBlendColor[i] - 1.f);
            }
        }
        
        vResault.a = vDiffuse.a;
    }
    else if (7 == iColorMode)
    {
        // 하드 라이트
        for (int i = 0; i < 3; ++i)
        {
            vResault[i] = (vBlendColor[i] < 0.5f) ? (2.f * vDiffuse[i] * vBlendColor[i]) :
                (1.f - 2.f * (1.f - vDiffuse[i]) * (1.f - vBlendColor[i]));
        }
        
        vResault.a = vDiffuse.a;
    }
    else if (8 == iColorMode)
    {
        // 컬러 닷지
        for (int i = 0; i < 3; ++i)
        {
            vResault[i] = (vBlendColor[i] == 1.f) ? vBlendColor[i] :
                min(vDiffuse[i] / (1.f - vBlendColor[i]), 1.f);

        }
        vResault.a = vDiffuse.a;
    }
    else if (9 == iColorMode)
    {
        // 혼합 번
        for (int i = 0; i < 3; ++i)
        {
            vResault[i] = (vBlendColor[i] == 1.f) ? vBlendColor[i] :
                max(1.f - ((1.f - vDiffuse[i]) / vBlendColor[i]), 0.f);

        }
        vResault.a = vDiffuse.a;
    }
   
 
    return vResault;

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


float4 Calculation_Distortion(float2 In_TexUV, float2 In_vTexcoord1, float2 In_vTexcoord2, float2 In_vTexcoord3)
{
    float4 vDistortionTex_1;
    float4 vDistortionTex_2;
    float4 vDistortionTex_3;
    
    float4 vDistortionTex_Final;
   
	
	// 디스토션 텍스쳐의 텍스쿠드를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산 x 3
    // 텍스쿠드에 곱하기는 크기 변화
    In_vTexcoord1 = (In_TexUV * g_vScales.x);
    In_vTexcoord2 = (In_TexUV * g_vScales.y);
    In_vTexcoord3 = (In_TexUV * g_vScales.z);
    
    
    if (0 == g_iScrollType)
    {
        // SCROLL_ROW : 가로 스크롤 
        In_vTexcoord1.x = In_vTexcoord1.x + (g_fFrameTime * g_vScrollSpeeds.x);
        In_vTexcoord2.x = In_vTexcoord2.x + (g_fFrameTime * g_vScrollSpeeds.y);
        In_vTexcoord3.x = In_vTexcoord3.x + (g_fFrameTime * g_vScrollSpeeds.z);     
    }
    else if (1 == g_iScrollType)
    {
        // SCROLL_COL : 세로 스크롤      
        In_vTexcoord1.y = In_vTexcoord1.y + (g_fFrameTime * g_vScrollSpeeds.x);
        In_vTexcoord2.y = In_vTexcoord2.y + (g_fFrameTime * g_vScrollSpeeds.y);
        In_vTexcoord3.y = In_vTexcoord3.y + (g_fFrameTime * g_vScrollSpeeds.z);
    }
    else if (2 == g_iScrollType)
    {
        // SCROLL_BOTH : 가로 + 세로 스크롤
        In_vTexcoord1 = In_vTexcoord1 + (g_fFrameTime * g_vScrollSpeeds.x);
        In_vTexcoord2 = In_vTexcoord2 + (g_fFrameTime * g_vScrollSpeeds.y);
        In_vTexcoord3 = In_vTexcoord3 + (g_fFrameTime * g_vScrollSpeeds.z);
    }
	else if (3 == g_iScrollType)
    {
        In_vTexcoord1 = RotateTexture(In_vTexcoord1, g_fFrameTime * g_vScrollSpeeds.x);
        In_vTexcoord2 = RotateTexture(In_vTexcoord2, g_fFrameTime * g_vScrollSpeeds.y);
        In_vTexcoord3 = RotateTexture(In_vTexcoord3, g_fFrameTime * g_vScrollSpeeds.z);
    }


    // 노이즈 텍스처로 디스토션(왜곡효과)를 만든다.
	// 셋 다 동일한 노이즈 텍스쳐이지만, 서로 다른 텍스쿠드를 사용하였기에 서로 다른 모양으로 샘플링된다.  
    vDistortionTex_1 = g_NoiseTexture.Sample(LinearSampler, In_vTexcoord1);
    vDistortionTex_2 = g_NoiseTexture.Sample(LinearSampler, In_vTexcoord2);
    vDistortionTex_3 = g_NoiseTexture.Sample(LinearSampler, In_vTexcoord3);

	
	// 디스토션 값의 범위를 (0, 1)에서 (-1, +1)이 되도록한다.
    vDistortionTex_1 = (vDistortionTex_1 - 0.5f) * 2.0f;
    vDistortionTex_2 = (vDistortionTex_2 - 0.5f) * 2.0f;
    vDistortionTex_3 = (vDistortionTex_3 - 0.5f) * 2.0f;

	
	// 디스토션의 xy값을 세 개의 서로 다른 왜곡xy좌표로 흩뜨린다.
    vDistortionTex_1.xy = vDistortionTex_1.xy * g_vDistortion1.xy;
    vDistortionTex_2.xy = vDistortionTex_2.xy * g_vDistortion2.xy;
    vDistortionTex_3.xy = vDistortionTex_3.xy * g_vDistortion3.xy;

	
	// 왜곡된 세 디스토션 값들을 하나의 디스토션으로 합성한다.
    vDistortionTex_Final = vDistortionTex_1 + vDistortionTex_2 + vDistortionTex_3;

    
    return vDistortionTex_Final;
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

    uint iInstanceID         : SV_INSTANCEID;
};

struct VS_OUT
{
	float4		vPosition	: SV_POSITION;
	float4		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		vWorldPos	: TEXCOORD1;
	float4		vProjPos	: TEXCOORD2;
    
    uint iInstanceID        : SV_INSTANCEID;
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
    
    uint iInstanceID        : SV_INSTANCEID;
};


struct PS_OUT
{	
	float4 vDiffuse			: SV_TARGET0;
    float4 vSolid			: SV_TARGET1;
	//float4 vNormal		: SV_TARGET2;
	//float4 vDepth			: SV_TARGET3;
	float4 vRimBloom		: SV_TARGET2;
    float4 vDistortion		: SV_TARGET3;
    
};


struct PS_OUT_EFFECT
{
    float4 vDiffuse     : SV_TARGET0;     // Diffuse
    float4 vSolid       : SV_TARGET1;
    float4 vNormal      : SV_TARGET2;      // Normal
    float4 vDepth       : SV_TARGET3;      // Depth
    float4 vRimBloom    : SV_TARGET4;      // RimBloom
    float4 vDistortion  : SV_TARGET5;
};



PS_OUT PS_MAIN(PS_IN In, uniform bool bSolid)
{
	PS_OUT Out = (PS_OUT) 0;
	
    float4 vFinalDiffuse;
    float4 vAlphaColor;
	
    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);
	

    vFinalDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vAlphaColor = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

    vFinalDiffuse.a *= vAlphaColor;
	
    if (vFinalDiffuse.a <= g_fAlpha_Discard)	// 알파 자르기
		discard;

    // 색상 혼합
    Out.vDiffuse = Calculation_ColorBlend(vFinalDiffuse, g_vColor_Mul, g_iColorMode);
	
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); /* -1 ~ 1 -> 0 ~ 1 */
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
   

    /* ---------------- New ---------------- */
	float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
	Out.vDiffuse += vRimColor;
    Out.vRimBloom = float4(g_vBloomPower, 1.0f); //Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;


    if (bSolid)
        Out.vSolid = Out.vDiffuse;
	

	return Out;

}
// MAIN =========================================================================================================================



// Dissolve (Normal Mapping(MAIN_NORMAL)) =======================================================================================
struct VS_OUT_NORMAL
{
	float4 vPosition	: SV_POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
	float4 vWorldPos	: TEXCOORD1;
	float4 vProjPos		: TEXCOORD2;
	
	float3 vTangent		: TANGENT;
	float3 vBinormal	: BINORMAL;
    
    uint iInstanceID    : SV_INSTANCEID;
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
    
    uint iInstanceID    : SV_INSTANCEID;
};


PS_OUT PS_MAIN_Dissolve(PS_IN_NORMAL In, uniform bool bSolid)
{
	PS_OUT Out = (PS_OUT)0;

    float4 vFinalDiffuse;
    float4 vAlphaColor;
	
     
    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);
	
	
    float2 vMaskUV = In.vTexUV * g_UVScale_Mask + g_UVOffset_Mask;
    if (false == g_bUV_Wave)
    {
        if ((vMaskUV.x > 1.f) || (vMaskUV.y > 1.f) || (vMaskUV.x < 0.f) || (vMaskUV.y < 0.f))
            discard;
    }
    else
    {
        vMaskUV.x = vMaskUV.x + (g_fFrameTime * g_UV_WaveSpeed.x);
        vMaskUV.y = vMaskUV.y + (g_fFrameTime * g_UV_WaveSpeed.y);
    }
     
    
    vFinalDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vAlphaColor = g_MaskTexture.Sample(LinearSampler, vMaskUV);

    vFinalDiffuse.a *= vAlphaColor;
	
	
	/* Discard & Color Mul ==================================================== */
    if (vFinalDiffuse.a <= g_fAlpha_Discard) // 알파 잘라내기
        discard;
	

   // 색상 혼합
    Out.vDiffuse.rgb = Calculation_ColorBlend(vFinalDiffuse, g_EffectDesc[In.iInstanceID].g_vColors_Mul, g_iColorMode).rgb;
    Out.vDiffuse.a = vFinalDiffuse.a * g_EffectDesc[In.iInstanceID].g_vColors_Mul.a * g_EffectDesc[In.iInstanceID].g_fCurAddAlpha;
 
	
	/* Dissolve ============================================================== */
    vector vDissolveTex = g_NoiseTexture.Sample(LinearSampler, In.vTexUV);
    clip(vDissolveTex - g_fDissolveRatio);

    float fStepValue = IsIn_Range(0.f, 0.05f, vDissolveTex.r - g_fDissolveRatio);

    vFinalDiffuse = (1.f - fStepValue) * vFinalDiffuse + fStepValue;

		
	/* Normal & Depth ========================================================= */
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vPixelNormal = mul(vPixelNormal, WorldMatrix);

    //Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    //Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

    
	
	/* RimBloom ================================================================ */
    float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    Out.vDiffuse += vRimColor;
    Out.vRimBloom = float4(g_vBloomPower, Out.vDiffuse.a) * g_EffectDesc[In.iInstanceID].g_fCurAddAlpha; //Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
	
	
	
    if (bSolid)
		Out.vSolid = Out.vDiffuse;
	
	
	return Out;
}
// Dissolve (Normal Mapping(MAIN_NORMAL)) =======================================================================================


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
    
    uint iInstanceID    : SV_INSTANCEID;
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
    
    uint iInstanceID    : SV_INSTANCEID;
};



PS_OUT PS_MAIN_DISTORTION(PS_IN_DISTORTION In, uniform bool bSolid)
{
	PS_OUT Out = (PS_OUT) 0;

    
	float4	vFinalDiffuse;
	float4	vAlphaColor;

    float4  vDistortion;
    float   fPerturb;
    float2  vDistortedCoord;
    
 
    // 텍스쿠드
    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);
	
    

    float2 vMaskUV = In.vTexUV * g_UVScale_Mask + g_UVOffset_Mask;
    if (false == g_bUV_Wave)
    {
        if ((vMaskUV.x > 1.f) || (vMaskUV.y > 1.f) || (vMaskUV.x < 0.f) || (vMaskUV.y < 0.f))
            discard;
    }
    else
    {
        vMaskUV.x = vMaskUV.x + (g_fFrameTime * g_UV_WaveSpeed.x);
        vMaskUV.y = vMaskUV.y + (g_fFrameTime * g_UV_WaveSpeed.y);
    }
    
    
    
    /* Distortion ============================================================ */	
    
    vDistortion = Calculation_Distortion(In.vTexUV, In.vTexcoord1, In.vTexcoord2, In.vTexcoord3);

	
	// 입력으로 들어온 텍스쳐의 Y좌표를 왜곡 크기와 바이어스 값으로 교란시킨다. 이 교란은 텍스쳐의 위쪽으로 갈수록 강해져서 맨 위쪽에는 깜박이는 효과를 만들어낸다.
	fPerturb = ((1.0f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	
	// 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를(UV) 만든다.
    vDistortedCoord = (vDistortion.xy * fPerturb) + In.vTexUV.xy;


	// 디퓨즈 텍스처 (clamp 샘플러 사용)
    vFinalDiffuse = g_DiffuseTexture.Sample(LinearSampler, vDistortedCoord.xy);


	// 마스크 텍스처를 알파로 사용 (clamp 샘플러 사용)
    vAlphaColor = g_MaskTexture.Sample(LinearSampler, vDistortedCoord.xy + vMaskUV);
    vFinalDiffuse.a *= vAlphaColor.r;
	
    
	/* Discard & Color Mul ==================================================== */
    if (vFinalDiffuse.a <= g_fAlpha_Discard) // 알파 잘라내기
        discard;
	
     // 색상 혼합
    Out.vDiffuse.rgb = Calculation_ColorBlend(vFinalDiffuse, g_EffectDesc[In.iInstanceID].g_vColors_Mul, g_iColorMode).rgb;
    Out.vDiffuse.a = vFinalDiffuse.a * g_EffectDesc[In.iInstanceID].g_vColors_Mul.a * g_EffectDesc[In.iInstanceID].g_fCurAddAlpha;
    
	/* Dissolve ============================================================== */
    vector vDissolveTex = g_NoiseTexture.Sample(LinearSampler, In.vTexUV);
    clip(vDissolveTex - g_fDissolveRatio);

    float fStepValue = IsIn_Range(0.f, 0.05f, vDissolveTex.r - g_fDissolveRatio);

    vFinalDiffuse = (1.f - fStepValue) * vFinalDiffuse + fStepValue;

		

	/* Normal & Depth ========================================================= */
	float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
	vPixelNormal = vPixelNormal * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

	vPixelNormal = mul(vPixelNormal, WorldMatrix);

	//Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
	
	
	/* RimBloom ================================================================ */
    float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    Out.vDiffuse += vRimColor;
    Out.vRimBloom = float4(g_vBloomPower, Out.vDiffuse.a) * g_EffectDesc[In.iInstanceID].g_fCurAddAlpha; //Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
	

    
    if (bSolid)
        Out.vSolid = Out.vDiffuse;
	
	
	return Out;
}
//  DISTORTION ==================================================================================================================



PS_OUT_EFFECT PS_MAIN_DISTORTION_NOTPRI(PS_IN_DISTORTION In, uniform bool bSolid)
{
    PS_OUT_EFFECT Out = (PS_OUT_EFFECT) 0;

    
    float4 vFinalDiffuse;
    float4 vAlphaColor;

    float4 vDistortion;
    float fPerturb;
    float2 vDistortedCoord;
    
 
    // 텍스쿠드
    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);
	
    

    float2 vMaskUV = In.vTexUV * g_UVScale_Mask + g_UVOffset_Mask;
    if (false == g_bUV_Wave)
    {
        if ((vMaskUV.x > 1.f) || (vMaskUV.y > 1.f) || (vMaskUV.x < 0.f) || (vMaskUV.y < 0.f))
            discard;
    }
    else
    {
        vMaskUV.x = vMaskUV.x + (g_fFrameTime * g_UV_WaveSpeed.x);
        vMaskUV.y = vMaskUV.y + (g_fFrameTime * g_UV_WaveSpeed.y);
    }
    
    
    
    /* Distortion ============================================================ */	
    
    vDistortion = Calculation_Distortion(In.vTexUV, In.vTexcoord1, In.vTexcoord2, In.vTexcoord3);

	
	// 입력으로 들어온 텍스쳐의 Y좌표를 왜곡 크기와 바이어스 값으로 교란시킨다. 이 교란은 텍스쳐의 위쪽으로 갈수록 강해져서 맨 위쪽에는 깜박이는 효과를 만들어낸다.
    fPerturb = ((1.0f - In.vTexUV.y) * g_fDistortionScale) + g_fDistortionBias;

	
	// 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를(UV) 만든다.
    vDistortedCoord = (vDistortion.xy * fPerturb) + In.vTexUV.xy;


	// 디퓨즈 텍스처 (clamp 샘플러 사용)
    vFinalDiffuse = g_DiffuseTexture.Sample(LinearSampler, vDistortedCoord.xy);


	// 마스크 텍스처를 알파로 사용 (clamp 샘플러 사용)
    vAlphaColor = g_MaskTexture.Sample(LinearSampler, vDistortedCoord.xy + vMaskUV);
    vFinalDiffuse.a *= vAlphaColor.r;
	
    
	/* Discard & Color Mul ==================================================== */
    if (vFinalDiffuse.a <= g_fAlpha_Discard) // 알파 잘라내기
        discard;
	
     // 색상 혼합
    Out.vDiffuse.rgb = Calculation_ColorBlend(vFinalDiffuse, g_EffectDesc[In.iInstanceID].g_vColors_Mul, g_iColorMode).rgb;
    Out.vDiffuse.a = vFinalDiffuse.a * g_EffectDesc[In.iInstanceID].g_vColors_Mul.a * g_EffectDesc[In.iInstanceID].g_fCurAddAlpha;
    
	/* Dissolve ============================================================== */
    vector vDissolveTex = g_NoiseTexture.Sample(LinearSampler, In.vTexUV);
    clip(vDissolveTex - g_fDissolveRatio);

    float fStepValue = IsIn_Range(0.f, 0.05f, vDissolveTex.r - g_fDissolveRatio);

    vFinalDiffuse = (1.f - fStepValue) * vFinalDiffuse + fStepValue;

		

	/* Normal & Depth ========================================================= */
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vPixelNormal = mul(vPixelNormal, WorldMatrix);

	Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
	
	
	/* RimBloom ================================================================ */
    float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    Out.vDiffuse += vRimColor;
    Out.vRimBloom = float4(g_vBloomPower, Out.vDiffuse.a) * g_EffectDesc[In.iInstanceID].g_fCurAddAlpha; //Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
	

    
    if (bSolid)
        Out.vSolid = Out.vDiffuse;
	
	
    return Out;
}
//  DISTORTION ==================================================================================================================




//  DISTORTION_POST ==================================================================================================
PS_OUT PS_MAIN_DISTORTION_POST(PS_IN_DISTORTION In)
{
    PS_OUT Out = (PS_OUT) 0;	
    
    In.vTexUV = In.vTexUV * g_UVScale + g_UVOffset;
    In.vTexUV = Rotate_Texcoord(In.vTexUV, g_fDegree);
    

	/* Distortion ============================================================ */ 
    float4 vDistortion;
    
    vDistortion = Calculation_Distortion(In.vTexUV, In.vTexcoord1, In.vTexcoord2, In.vTexcoord3);
    Out.vDistortion = vDistortion;
    
    
	/* Depth ================================================================ */
    //Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
	
	
	/* RimBloom ============================================================== */
    Out.vDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    Out.vRimBloom = float4(g_vBloomPower, 1.0f); //Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
	
      
    return Out;
}
//  DISTORTION_POST ==================================================================================================



technique11 DefaultTechnique
{
    pass WireFrame // 0
    {
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_NoneCull_Wireframe);

        VertexShader	= compile vs_5_0 VS_MAIN();
        HullShader		= NULL;
        DomainShader	= NULL;
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_MAIN(true);
    }

    pass Effect // 1
    {	
		//SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff); //SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);	
		SetRasterizerState(RS_Cull_None); //SetRasterizerState(RS_Default);

        VertexShader	= compile vs_5_0 VS_MAIN_NORMAL();
		HullShader		= NULL;
		DomainShader	= NULL;
		GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_MAIN_Dissolve(false);
    }

    pass Effect_Solid // 2
    {
		//SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff); //SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None); //SetRasterizerState(RS_Default);

        VertexShader	= compile vs_5_0 VS_MAIN_NORMAL();
        HullShader		= NULL;
        DomainShader	= NULL;
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_MAIN_Dissolve(true);
    }

	pass Distortion // 3
	{
		SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthStencilEnable, 0);
		SetRasterizerState(RS_Cull_None);

		VertexShader	= compile vs_5_0 VS_MAIN_DISTORTION();
		HullShader		= NULL;
		DomainShader	= NULL;
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN_DISTORTION(false);
	}

    pass Distortion_Solid // 4
    {
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader	= compile vs_5_0 VS_MAIN_DISTORTION();
        HullShader		= NULL;
        DomainShader	= NULL;
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_MAIN_DISTORTION(true);
    }

    pass Distortion_Post // 5
    {
        //SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader	= compile vs_5_0 VS_MAIN_DISTORTION();
        HullShader		= NULL;
        DomainShader	= NULL;
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_MAIN_DISTORTION_POST();
    }

    pass Distortion_NotPri // 6
    {
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_NOTPRI(false);
    }

    pass Distortion_Solid_NotPri // 7
    {
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetRasterizerState(RS_Cull_None);

        VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION_NOTPRI(true);
    }


}