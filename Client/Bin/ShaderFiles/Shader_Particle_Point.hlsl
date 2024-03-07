#include "Shader_Defines.hlsli"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DepthTexture;

Texture2D	g_DiffuseTexture;
Texture2D	g_MaskTexture;
Texture2D	g_NoiseTexture;

// Camera ====================
vector		g_vCamPosition;
float3		g_vCamDirection;
float		g_fCamFar;
// ===========================

bool        g_bBillBoard;
float       g_fAlpha_Discard;
float3      g_vBlack_Discard;

float		g_fDegree;

// Sprite ====================
bool		g_bSprite;
float2		g_UVOffset;
float2		g_UVScale;
// ===========================


struct EffectDesc
{
	float3	g_vDir;
	float	g_Padding;
};
EffectDesc g_EffectDesc[500];


/* Custom Function */
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

// 두 벡터 사이의 각도 계산 (라디안)
float Calculate_AngleBetweenVectors_Radian(float3 v1, float3 v2)
{
	float fDot = dot(normalize(v1), normalize(v2));
	float fAngle = acos(fDot);

	return fAngle;
}

// 두 벡터 사이의 각도 계산 (도 Degree)
float Calculate_AngleBetweenVectors_Degree(float3 v1, float3 v2)
{
	float fRadians = Calculate_AngleBetweenVectors_Radian(v1, v2);
	float fDegree = fRadians * (180.0f / 3.14159265358979323846);

	return fDegree;
}


struct VS_IN
{
	float3				vPosition : POSITION;
	float2				vPSize : PSIZE;

	row_major float4x4	TransformMatrix : WORLD;
	float4				vColor : COLOR0;

	uint				iInstanceID : SV_INSTANCEID;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
	float4		vColor : COLOR0;

	uint	    iInstanceID : SV_INSTANCEID;
};



VS_OUT VS_MAIN_PARTICLE(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = float2(In.vPSize.x * In.TransformMatrix._11, In.vPSize.y * In.TransformMatrix._22);
	Out.vColor = In.vColor;

	Out.iInstanceID = In.iInstanceID;

	return Out;
}


struct GS_IN
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
	float4		vColor : COLOR0;

	uint	iInstanceID : SV_INSTANCEID;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vColor : COLOR0;
};

/* 지오메트리 쉐이더 : 셰이더안에서 정점을 추가적으로 생성해 준다. */
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT		Out[4];

	float4		vLook;
	float3		vRight, vUp;
	if (g_bBillBoard)
	{
		vLook = g_vCamPosition - In[0].vPosition;
		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
		vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;
	}
	else
	{
		// 이동 진행 방향벡터를 Up으로 한 새로운 Right, Look 정해주기 ===================================
		vUp = normalize(g_EffectDesc[In[0].iInstanceID].g_vDir).rgb * In[0].vPSize.y * 0.5f;
		vLook.rgb = normalize(cross(float3(0.f, 1.f, 0.f), vUp));
		vRight = normalize(cross(vUp, vLook.rgb)) * In[0].vPSize.x * 0.5f;
		vLook.rgb = normalize(cross(vRight, vUp)); vLook.a = 0.f;
	}

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	// 중앙 원점이 아님! (중앙 위로 원점 바꿔서 찍었음!!)
	//Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
	Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight, 1.f), matVP);
	Out[0].vTexcoord = Rotate_Texcoord(float2(0.f, 0.f), g_fDegree);
	Out[0].vColor = In[0].vColor;

	//Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
	Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight, 1.f), matVP);
	Out[1].vTexcoord = Rotate_Texcoord(float2(1.f, 0.f), g_fDegree);
	Out[1].vColor = In[0].vColor;

	//Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
	Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - (vUp * 2), 1.f), matVP);
	Out[2].vTexcoord = Rotate_Texcoord(float2(1.f, 1.f), g_fDegree);
	Out[2].vColor = In[0].vColor;

	//Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
	Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - (vUp * 2), 1.f), matVP);
	Out[3].vTexcoord = Rotate_Texcoord(float2(0.f, 1.f), g_fDegree);
	Out[3].vColor = In[0].vColor;


	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);
	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);
	OutStream.RestartStrip();
}

/* 통과된 정점을 대기 .*/

/* 투영변환 : /w */ /* -> -1, 1 ~ 1, -1 */
/* 뷰포트변환-> 0, 0 ~ WINSX, WINSY */
/* 래스터라이즈 : 정점정보에 기반하여 픽셀의 정보를 만든다. */


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
	float4		vColor : COLOR0;

};

struct PS_OUT
{
	float4		vColor		: SV_TARGET0; // Diffuse
	float4		vNormal		: SV_TARGET1; // Normal
	float4		vDepth		: SV_TARGET2; // Depth
	float4		vRimBloom	: SV_TARGET3; // RimBloom
};


/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
PS_OUT PS_MAIN_PARTICLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (g_bSprite)
	{
		float2 clippedTexCoord = In.vTexcoord * g_UVScale + g_UVOffset;
		float4 vDiffuseColor = g_DiffuseTexture.Sample(PointSampler, clippedTexCoord);

		vDiffuseColor.rgb *= In.vColor.rgb;

		if (vDiffuseColor.a < g_fAlpha_Discard	// 알파 잘라내기
		|| vDiffuseColor.r < g_vBlack_Discard.r && vDiffuseColor.g < g_vBlack_Discard.g && vDiffuseColor.b < g_vBlack_Discard.b)	// 검정색 잘라내기
			discard;

		Out.vColor = vDiffuseColor;

    }
	else
	{
		/* 첫번째 인자의 방식으로 두번째 인자의 위치에 있는 픽셀의 색을 얻어온다. */
		float4 vDiffuseColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
		float4 vAlphaColor = g_MaskTexture.Sample(PointSampler, In.vTexcoord);

		vDiffuseColor.rgb *= In.vColor.rgb;
		vDiffuseColor.a = In.vColor.a * vAlphaColor;

		if (vDiffuseColor.a < g_fAlpha_Discard	// 알파 잘라내기
			|| vDiffuseColor.r < g_vBlack_Discard.r && vDiffuseColor.g < g_vBlack_Discard.g && vDiffuseColor.b < g_vBlack_Discard.b)	// 검정색 잘라내기
			discard;

        Out.vColor = vDiffuseColor;
    }


    return Out;
}

technique11 DefaultTechnique
{
	/* 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
	pass Particle  // 0
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		/* 렌더스테이츠 */
		VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLE();
	}


	pass Bloom  // 1
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		/* 렌더스테이츠 */
		VertexShader = compile vs_5_0 VS_MAIN_PARTICLE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PARTICLE();
	}

}