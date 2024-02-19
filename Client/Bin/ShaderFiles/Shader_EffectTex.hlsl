#include "Shader_Defines.hlsli"

/* float2, float3, float4 == vector */
/* vector.x == vector.r */
/* vector.xy = 1.f;*/
/* float4x4 == matrix */
/* matrix._11_12 = 1.f; */

/* 셰이더의 전역변수 == 상수테이블(Constant Table) */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;
texture2D		g_MaskTexture;
texture2D		g_NoiseTexture;

texture2D		g_DepthTexture;

vector			g_vCamPosition;
vector			g_vCamDirection;

float2			g_UVOffset;
float2			g_UVScale;


// ======= Noise
float	g_fFrameTime;
float3	g_vScrollSpeeds;
float3	g_vScales;
// =======


float2	g_vDistortion1;
float2	g_vDistortion2;
float2	g_vDistortion3;
float	g_fDistortionScale;
float	g_fDistortionBias;




#pragma region STRUCT
/* ========================= IN OUT ========================= */

/* ========================= VS_IN ========================= */

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};

/* ========================= VS_OUT ========================= */

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;	
};

struct VS_OUT_EFFECT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct VS_OUT_NOISE
{
	float4		vPosition	: SV_POSITION;

	float2		vTexcoord	: TEXCOORD0;
	float2		vTexcoord1	: TEXCOORD1;
	float2		vTexcoord2	: TEXCOORD2;
	float2		vTexcoord3	: TEXCOORD3;

	float4		vProjPos : TEXCOORD4;
};

/* ========================= PS_IN ========================= */

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};


struct PS_IN_EFFECT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_IN_NOISE
{
	float4		vPosition	: SV_POSITION;

	float2		vTexcoord	: TEXCOORD0;
	float2		vTexcoord1	: TEXCOORD1;
	float2		vTexcoord2	: TEXCOORD2;
	float2		vTexcoord3	: TEXCOORD3;

	float4		vProjPos : TEXCOORD4;
};

/* ========================= PS_OUT ========================= */

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};
#pragma endregion STRUCT

/* ========================= VS_OUT ========================= */

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	/* In.vPosition * 월드 * 뷰 * 투영 */
	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}


VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
	VS_OUT_EFFECT		Out = (VS_OUT_EFFECT)0;

	/* In.vPosition * 월드 * 뷰 * 투영 */
	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;
}


VS_OUT_EFFECT  VS_MAIN_SPRITE(VS_IN In)
{
	VS_OUT_EFFECT Out = (VS_OUT_EFFECT)0;

	matrix WorldMatrix = g_WorldMatrix;

	float3 vLook = normalize((g_vCamDirection * -1.f).xyz);
	float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
	float3 vUp = normalize(cross(vLook, vRight));

	WorldMatrix[0] = float4(vRight, 0.f) * length(WorldMatrix[0]);
	WorldMatrix[1] = float4(vUp, 0.f) * length(WorldMatrix[1]);
	WorldMatrix[2] = float4(vLook, 0.f) * length(WorldMatrix[2]);

	/* In.vPosition * 월드 * 뷰 * 투영 */
	matrix matWV, matWVP;

	matWV = mul(WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT_NOISE VS_MAIN_NOISE(VS_IN In)
{
	VS_OUT_NOISE		Out = (VS_OUT_NOISE)0;

	/* In.vPosition * 월드 * 뷰 * 투영 */
	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;
	

	// 노이즈 텍스쳐의 좌표를 첫번째 크기 및 윗방향 스크롤 속도 값을 이용하여 계산 x 3
	Out.vTexcoord1 = (In.vTexcoord * g_vScales.x);
	Out.vTexcoord1.y = Out.vTexcoord1.y + (g_fFrameTime * g_vScrollSpeeds.x);

	Out.vTexcoord2 = (In.vTexcoord * g_vScales.y);
	Out.vTexcoord2.y = Out.vTexcoord2.y + (g_fFrameTime * g_vScrollSpeeds.y);

	Out.vTexcoord3 = (In.vTexcoord * g_vScales.z);
	Out.vTexcoord3.y = Out.vTexcoord3.y + (g_fFrameTime * g_vScrollSpeeds.z);


	return Out;
}

/* ========================= PS_OUT ========================= */

/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	///* 첫번째 인자의 방식으로 두번째 인자의 위치에 있는 픽셀의 색을 얻어온다. */
	//vector		vSourColor = g_Texture[0].Sample(LinearSampler, In.vTexcoord);
	//vector		vDestColor = g_Texture[1].Sample(LinearSampler, In.vTexcoord);

	//Out.vColor = vSourColor + vDestColor;

	return Out;
}

PS_OUT PS_MAIN_EFFECT(PS_IN_EFFECT In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	float2	vDepthTexcoord;
	vDepthTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vDepthTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4	vDepthDesc = g_DepthTexture.Sample(PointSampler, vDepthTexcoord);
	
	Out.vColor.a = Out.vColor.a * (vDepthDesc.y * 1000.f - In.vProjPos.w) * 2.f;

	return Out;
}


PS_OUT PS_MAIN_SPRITE_ANIMATION(PS_IN_EFFECT In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 clippedTexCoord = In.vTexcoord * g_UVScale + g_UVOffset;

	// Set Color
	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, clippedTexCoord);

	float2 vDepthTexcoord;
	vDepthTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vDepthTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vDepthTexcoord);

	Out.vColor.a = Out.vColor.a * (vDepthDesc.y * 1000.f - In.vProjPos.w) * 2.f;
	// Alpha Test
	if (Out.vColor.a < 0.9f)
	{
		discard;
	}

	return Out;
}

PS_OUT PS_MAIN_NOISE(PS_IN_NOISE In)
{
	PS_OUT Out = (PS_OUT)0;

	float4  vNoise1;
	float4  vNoise2;
	float4  vNoise3;

	float4	vFinalNoise;
	float	fPerturb;
	float2	vNoiseCoords;
	float4	vFireColor;
	float4	vAlphaColor;


	// 동일한 노이즈 텍스쳐를 서로 다른 세 텍스쳐 좌표를 사용하여 세 개의 다른 크기의 노이즈를 얻는다.
	vNoise1 = g_NoiseTexture.Sample(ClampSampler, In.vTexcoord1);
	vNoise2 = g_NoiseTexture.Sample(ClampSampler, In.vTexcoord2);
	vNoise3 = g_NoiseTexture.Sample(ClampSampler, In.vTexcoord3);

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
	fPerturb = ((1.0f - In.vTexcoord.y) * g_fDistortionScale) + g_fDistortionBias;

	// 불꽃 색상 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를 만든다.
	vNoiseCoords.xy = (vFinalNoise.xy * fPerturb) + In.vTexcoord.xy;


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 불꽃 텍스쳐에서 색상을 샘플링한다.
	// wrap를 사용하는 스테이트 대신 clamp를 사용하는 스테이트를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
	vFireColor = g_DiffuseTexture.Sample(ClampSampler, vNoiseCoords.xy);


	// 왜곡되고 교란된 텍스쳐 좌표를 이용하여 알파 텍스쳐에서 알파값을 샘플링한다. (불꽃의 투명도를 지정하는 데 사용)
	// wrap를 사용하는 스테이트 대신 clamp를 사용하는 스테이트를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지한다.
	vAlphaColor = g_MaskTexture.Sample(ClampSampler, vNoiseCoords.xy);

	vFireColor.a = vAlphaColor;

	Out.vColor = vFireColor;

	return Out;
}

technique11 DefaultTechnique
{
	/* 위와 다른 형태에 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
	pass Effect // 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
	}	

	pass Sprite // 1
	{
		SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthStencilEnable, 0);
		SetRasterizerState(RS_Cull_None);

		VertexShader = compile vs_5_0 VS_MAIN_SPRITE();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SPRITE_ANIMATION();
	}

	pass Noise // 1
	{
		SetBlendState(BS_AlphaBlend_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthStencilEnable, 0);
		SetRasterizerState(RS_Cull_None);

		VertexShader = compile vs_5_0 VS_MAIN_NOISE();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NOISE();
	}
}
