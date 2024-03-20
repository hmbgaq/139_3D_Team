#include "Shader_Defines.hlsli"

/* float2, float3, float4 == vector */
/* vector.x == vector.r */
/* vector.xy = 1.f;*/
/* float4x4 == matrix */
/* matrix._11_12 = 1.f; */

/* 셰이더의 전역변수 == 상수테이블(Constant Table) */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D		g_DepthTexture;

Texture2D		g_DiffuseTexture;
Texture2D		g_MaskTexture;
Texture2D		g_NoiseTexture;


// Camera ====================
vector		g_vCamPosition;
float3		g_vCamDirection;
float		g_fCamFar;
// ===========================


// Sprite ====================
bool	g_bSprite;
float2	g_UVOffset;
float2	g_UVScale;
// ===========================


bool	g_bBillBoard;

float	g_fAlpha_Discard;
float3	g_vBlack_Discard;
float4	g_vBloom_Discard;

/* Color */
int		g_iColorMode;
float4	g_vColor_Mul;

float	g_fDegree;


/* RimLight - 필요한거 두개뿐임(+1개 추가) */
float4	g_vRimColor;
float	g_fRimPower;
float3	g_vBloomPower;


// Distortion ====================
float	g_fFrameTime;

int		g_iScrollType;

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


float4 Calculation_ColorBlend(float4 vDiffuse, float4 vBlendColor)
{
    float4 vResault = vDiffuse;
	
    if (0 == g_iColorMode)
    {
		// 곱하기
        vResault = vResault * vBlendColor;
    }
    else if (1 == g_iColorMode)
    {
		// 스크린
        vResault = 1.f - ((1.f - vResault) * (1.f - vBlendColor));
    }
    else if (2 == g_iColorMode)
    {
		// 오버레이
        vResault = max(vResault, vBlendColor);
    }
    else if (3 == g_iColorMode)
    {
		// 더하기
        vResault = vResault + vBlendColor;
    }
    else if (4 == g_iColorMode)
    {
		// 번(Burn)
        vResault = vResault + vBlendColor - 1.f;
    }
	
 
    return vResault;
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




// MAIN_EFFECT ==================================================================================================================
struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};


struct VS_OUT
{
	float4		vPosition	: SV_POSITION;
	float2		vTexcoord	: TEXCOORD0;
	float4		vProjPos	: TEXCOORD1;
};


VS_OUT VS_MAIN_EFFECT(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

    matrix WorldMatrix = g_WorldMatrix;

	/* In.vPosition * 월드 * 뷰 * 투영 */
    matrix matWV, matWVP;
	
	if(g_bBillBoard)
    {	
        float3 vLook	= normalize((g_vCamDirection * -1.f).xyz);
        float3 vRight	= normalize(cross(float3(0.f, 1.f, 0.f), vLook));
        float3 vUp		= normalize(cross(vLook, vRight));

        WorldMatrix[0] = float4(vRight, 0.f) * length(WorldMatrix[0]);
        WorldMatrix[1] = float4(vUp, 0.f) * length(WorldMatrix[1]);
        WorldMatrix[2] = float4(vLook, 0.f) * length(WorldMatrix[2]);
    }
 
    matWV = mul(WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;
}


struct PS_IN
{
	float4 vPosition	: SV_POSITION;
	float2 vTexcoord	: TEXCOORD0;
	float4 vProjPos		: TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor		: SV_TARGET0; // Diffuse
    float4 vSolid		: SV_TARGET1;
    float4 vNormal		: SV_TARGET2; // Normal
    float4 vDepth		: SV_TARGET3; // Depth
    float4 vRimBloom	: SV_TARGET4; // RimBloom
};


PS_OUT PS_MAIN_EFFECT(PS_IN In, uniform bool bSolid)
{
	PS_OUT Out = (PS_OUT) 0;

    float4 vFinalDiffuse;
    float4 vAlphaColor;
	
    In.vTexcoord = In.vTexcoord * g_UVScale + g_UVOffset;
    In.vTexcoord = Rotate_Texcoord(In.vTexcoord, g_fDegree);
	
	
    vFinalDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vAlphaColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);

    vFinalDiffuse.a *= vAlphaColor;

	
	/* Discard & Color Mul ==================================================== */
    if (vFinalDiffuse.a <= g_fAlpha_Discard) // 알파 잘라내기
        discard;

	// 검은색(or 특정 걸러) 잘라내기
    //if (Out.vColor.r <= g_vBlack_Discard.r && Out.vColor.g <= g_vBlack_Discard.g && Out.vColor.b <= g_vBlack_Discard.b)
    //    discard;
	
	// 컬러 혼합
    Out.vColor = Calculation_ColorBlend(vFinalDiffuse, g_vColor_Mul);
	
	
	/* ---------------- Rim Bloom ---------------- :  */	
    //float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    //Out.vDiffuse += vRimColor;
	Out.vRimBloom = float4(g_vBloomPower, 1.0f);	//Out.vRimBloom = Calculation_Brightness(Out.vColor);
	

	
    if (bSolid)
        Out.vSolid = Out.vColor;
	
	
	return Out;
}
// MAIN_EFFECT ==================================================================================================================



// MAIN_DISTORTION ==============================================================================================================
struct VS_OUT_DISTORTION
{	
    float4 vPosition	: SV_POSITION;
    float2 vTexcoord	: TEXCOORD0;
    float4 vProjPos		: TEXCOORD1;
	
    float2 vTexcoord1	: TEXCOORD2;
    float2 vTexcoord2	: TEXCOORD3;
    float2 vTexcoord3	: TEXCOORD4;
	
};


struct PS_IN_DISTORTION
{
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord    : TEXCOORD0;
    float4 vProjPos     : TEXCOORD1;
	
    float2 vTexcoord1   : TEXCOORD2;
    float2 vTexcoord2   : TEXCOORD3;
    float2 vTexcoord3   : TEXCOORD4;
};


VS_OUT_DISTORTION VS_MAIN_DISTORTION(VS_IN In)
{	
    VS_OUT_DISTORTION Out = (VS_OUT_DISTORTION) 0;

    matrix WorldMatrix = g_WorldMatrix;
	
	/* In.vPosition * 월드 * 뷰 * 투영 */
    matrix matWV, matWVP;
	
    if (g_bBillBoard)
    {
        float3 vLook = normalize((g_vCamDirection * -1.f).xyz);
        float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
        float3 vUp = normalize(cross(vLook, vRight));

        WorldMatrix[0] = float4(vRight, 0.f) * length(WorldMatrix[0]);
        WorldMatrix[1] = float4(vUp, 0.f) * length(WorldMatrix[1]);
        WorldMatrix[2] = float4(vLook, 0.f) * length(WorldMatrix[2]);
    }
 
    matWV = mul(WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
	
}


PS_OUT PS_MAIN_DISTORTION(PS_IN_DISTORTION In, uniform bool bSolid)
{
	PS_OUT Out = (PS_OUT) 0;

  
    float4 vFinalDiffuse;
    float4 vAlphaColor;

    float4 vDistortion;
    float fPerturb;
    float2 vDistortedCoord;
    
    
    // 텍스쿠드
    In.vTexcoord = In.vTexcoord * g_UVScale + g_UVOffset;
    In.vTexcoord = Rotate_Texcoord(In.vTexcoord, g_fDegree);
    
    /* Distortion ============================================================ */	
    
    vDistortion = Calculation_Distortion(In.vTexcoord, In.vTexcoord1, In.vTexcoord2, In.vTexcoord3);

	// 입력으로 들어온 텍스쳐의 Y좌표를 왜곡 크기와 바이어스 값으로 교란시킨다. 이 교란은 텍스쳐의 위쪽으로 갈수록 강해져서 맨 위쪽에는 깜박이는 효과를 만들어낸다.
    fPerturb = ((1.0f - In.vTexcoord.y) * g_fDistortionScale) + g_fDistortionBias;

    
	// 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를(UV) 만든다.
    vDistortedCoord = (vDistortion.xy * fPerturb) + In.vTexcoord.xy;


	// 디퓨즈 텍스처 (clamp 샘플러 사용)
    vFinalDiffuse = g_DiffuseTexture.Sample(ClampSampler, vDistortedCoord.xy);


	// 마스크 텍스처를 알파로 사용 (clamp 샘플러 사용)
    vAlphaColor = g_MaskTexture.Sample(ClampSampler, vDistortedCoord.xy);
    vFinalDiffuse.a *= vAlphaColor;

	/* Discard & Color Mul ==================================================== */
    if (vFinalDiffuse.a <= g_fAlpha_Discard) // 알파 잘라내기
        discard;
	
     // 색상 혼합
    Out.vColor = Calculation_ColorBlend(vFinalDiffuse, g_vColor_Mul);

		
    /* RimBloom ================================================================ */
    //float4 vRimColor = Calculation_RimColor(float4(In.vNormal.r, In.vNormal.g, In.vNormal.b, 0.f), In.vWorldPos);
    //Out.vColor += vRimColor;
    Out.vRimBloom = float4(g_vBloomPower, 1.0f); //Out.vRimBloom = Calculation_Brightness(Out.vDiffuse) /*+ vRimColor*/;
    
	
    if (bSolid)
		Out.vSolid = Out.vColor;
    
	
	return Out;
}
// MAIN_DISTORTION ==============================================================================================================




// MAIN_WIREFRAME ===============================================================================================================
PS_OUT PS_MAIN_WIREFRAME(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
    float4 vFinalDiffuse;
 
    In.vTexcoord = In.vTexcoord * g_UVScale + g_UVOffset;
    In.vTexcoord = Rotate_Texcoord(In.vTexcoord, g_fDegree);
	
    vFinalDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	
	
    Out.vColor = Calculation_ColorBlend(vFinalDiffuse, g_vColor_Mul);

	return Out;
}
// MAIN_WIREFRAME ===============================================================================================================




technique11 DefaultTechnique
{
	/* 위와 다른 형태에 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
	pass Effect // 0
	{
		SetRasterizerState(RS_Cull_None); //SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT(false);
	}	

    pass Effect_Solid // 1
    {
        SetRasterizerState(RS_Cull_None); //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT(true);
    }

	pass Distortion // 2
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_DepthStencilEnable, 0);
		SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION(false);
	}

    pass Distortion_Solid // 3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthStencilEnable, 0);
        SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_DISTORTION();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION(true);
    }


	pass Effect_Wireframe // 4
	{
		SetRasterizerState(RS_NoneCull_Wireframe);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_WIREFRAME();
	}

}
