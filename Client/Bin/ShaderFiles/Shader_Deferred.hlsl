#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ProjMatrixInv, g_ViewMatrixInv;
matrix			g_LightViewMatrix, g_LightProjMatrix;
matrix			g_CamProjMatrix, g_CamViewMatrix; /* ssr에서 사용 */ 
float			g_CamFar;

vector			g_vLightDir;
vector			g_vLightPos;
float			g_fLightRange;

vector			g_vLightDiffuse;	
vector			g_vLightAmbient;
vector			g_vLightSpecular;
float			g_fLightIntensity;
vector			g_vLightFlag;

texture2D		g_DiffuseTexture;
vector			g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vCamPosition;

Texture2D		g_PriorityTarget;
Texture2D		g_ShadeTexture;
Texture2D		g_NormalTexture;
Texture2D		g_NormalDepthTarget;
Texture2D		g_DepthTexture;
Texture2D		g_SpecularTexture;
Texture2D		g_LightDepthTexture;
Texture2D		g_ORMTexture;
Texture2D		g_SSAOTexture;
Texture2D		g_BloomTarget;
Texture2D		g_OutlineTarget;

/* 활성 여부 */ 
bool			g_bSSAO_Active;
bool			g_bBloom_Active;
bool			g_Outline_Active;
bool			g_PBR_Active;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

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


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;	
};

/* ------------------ 0 - DEBUG ------------------ */

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

/* ------------------ 1 - Directional ------------------ */

struct PS_OUT_LIGHT
{
	float4		vShade : SV_TARGET0;
	float4		vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vDiffuseColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
	vector		vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector		vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector		vORMDesc = g_ORMTexture.Sample(PointSampler, In.vTexcoord); /* Occlusion , Roughness, Metalic */
	//vector		
	
    vDiffuseColor = pow(vDiffuseColor, 2.2f);
    vDiffuseColor.a = 1.f;
	/* 0, 1 -> -1, 1 */
	float4		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);	

	Out.vShade = g_vLightDiffuse * min((max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)), 1.f);

    float		fViewZ = vDepthDesc.y * g_CamFar;

	vector		vWorldPos;

	/* 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬* 투영행렬 / View.z */
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLook = vWorldPos - g_vCamPosition;
	vector		vReflect = reflect(normalize(g_vLightDir), vNormal);	

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	return Out;
}

/* ------------------ 2 - Point ------------------ */

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);

	/* 0, 1 -> -1, 1 */
	float4		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	float		fViewZ = vDepthDesc.y * 1000.f;

	vector		vWorldPos;

	/* 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬* 투영행렬 / View.z */
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


	float4		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);

	float		fAtt = max((g_fLightRange - fDistance) / g_fLightRange, 0.f);

	Out.vShade = g_vLightDiffuse * min((max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)), 1.f) * fAtt;

	
	vector		vLook = vWorldPos - g_vCamPosition;
	vector		vReflect = reflect(normalize(vLightDir), vNormal);

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f) * fAtt;

	return Out;
}


/* ------------------ 3 - Spot  ------------------ */
PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 1;

	/* 방향성광원의 정보와 노멀 타겟에 담겨있는 노멀과의 빛연산을 수행한다. */
    vector vDiffuseColor	= g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormalDesc		= g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc		= g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vORMDesc			= g_ORMTexture.Sample(PointSampler, In.vTexcoord);

    return Out;
}

/* ------------------ 4 - Deferred ------------------ */

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vDiffuse.a == 0.f)
    {
        float4 vPriority = g_PriorityTarget.Sample(LinearSampler, In.vTexcoord);
        Out.vColor = vPriority;
        return Out;
    }
	
	vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vShade = saturate(vShade);
	
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
		vSpecular = saturate(vSpecular);
	
    vector		vSSAO = float4(1.f, 1.f, 1.f, 1.f);
    if(g_bSSAO_Active)
        vSSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexcoord); /* SSAO 적용 */
	
    vector vBloom = float4(0.f, 0.f, 0.f, 0.f);
	if(g_bBloom_Active)
        vBloom = g_BloomTarget.Sample(LinearSampler, In.vTexcoord);
	
    vector vOutline = float4(1.f, 1.f, 1.f, 1.f);
    if (g_Outline_Active)
        vOutline = g_OutlineTarget.Sample(LinearSampler, In.vTexcoord);
	
    Out.vColor = (vDiffuse * vShade * vSSAO) + vSpecular + vBloom;
	
    //Out.vColor = ((vDiffuse * vShade * vSSAO) + vSpecular + vBloom) * vOutline;
	
	vector		vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float		fViewZ = vDepthDesc.y * g_CamFar;
	
	vector		vWorldPos;

	/* 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬* 투영행렬 / View.z */
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 월드 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
	vWorldPos = mul(vWorldPos, g_LightViewMatrix);
	vWorldPos = mul(vWorldPos, g_LightProjMatrix);

	float2		vUV = (float2)0.0f;

	vUV.x = (vWorldPos.x / vWorldPos.w) * 0.5f + 0.5f;
	vUV.y = (vWorldPos.y / vWorldPos.w) * -0.5f + 0.5f;

	float4		vLightDepth = g_LightDepthTexture.Sample(LinearSampler, vUV);

	if (vWorldPos.w - 0.1f > vLightDepth.x * 300.f)
		Out.vColor = Out.vColor * 0.8f;
	
	return Out;
}

/* ------------------ 5 - PBR Deferred ------------------ */

PS_OUT PS_MAIN_PBR_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
	
	
	
    return Out;
}

/* ------------------ Technique ------------------ */

technique11 DefaultTechnique
{
	pass Debug // 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Directional // 1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Blend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point // 2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Blend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

    pass Light_Spot // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOT();
    }

	pass Deferred // 4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FINAL();
	}

    pass PBR_Deferred // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PBR_DEFERRED();
    }
}
