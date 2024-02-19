#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;
texture2D		g_MaskTexture;
texture2D		g_NoiseTexture;

vector			g_vCamPosition;
vector			g_vCamDirection;

float			g_fDegree;


cbuffer FX_Variables
{
    float2	vUV_Offset		= float2(0.f, 0.f);
    bool	bUV_Wave		= false;
    float	fUV_WaveSpeed	= 1.f;
    float2	vUV_TileCount	= float2(1.f, 1.f);
    float2	vUV_TileIndex	= float2(0.f, 0.f);
    float4	vColor_Offset	= float4(0.f, 0.f, 0.f, 0.f);
    float4	vColor_Clip		= float4(0.f, 0.f, 0.f, 0.f);
    float4	vColor_Mul		= float4(1.f, 1.f, 1.f, 1.f);
};


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


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;

	row_major float4x4	TransformMatrix : WORLD;
	float4		vColor : COLOR0;
};


struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
	float4		vColor : COLOR0;
};



VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = float2(In.vPSize.x * In.TransformMatrix._11, In.vPSize.y * In.TransformMatrix._22);
	Out.vColor = In.vColor;

	return Out;
}


struct GS_IN
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
	float4		vColor : COLOR0;
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

	float4		vLook = g_vCamPosition - In[0].vPosition;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(float4(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
	Out[0].vTexcoord = Rotate_Texcoord(float2(0.f, 0.f), g_fDegree);
	Out[0].vColor = In[0].vColor;

	Out[1].vPosition = mul(float4(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
	Out[1].vTexcoord = Rotate_Texcoord(float2(1.f, 0.f), g_fDegree);
	Out[1].vColor = In[0].vColor;

	Out[2].vPosition = mul(float4(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
	Out[2].vTexcoord = Rotate_Texcoord(float2(1.f, 1.f), g_fDegree);
	Out[2].vColor = In[0].vColor;

	Out[3].vPosition = mul(float4(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
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
	float4		vColor : SV_TARGET0;
};

/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	/* 첫번째 인자의 방식으로 두번째 인자의 위치에 있는 픽셀의 색을 얻어온다. */
    Out.vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);

    if (Out.vColor.a < 0.5f)
        discard;

    Out.vColor.rgb *= In.vColor.rgb;

    Out.vColor.a = In.vColor.a;

	return Out;
}


technique11 DefaultTechnique
{
	/* 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
	pass Particle  //0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		/* 렌더스테이츠 */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}


}
