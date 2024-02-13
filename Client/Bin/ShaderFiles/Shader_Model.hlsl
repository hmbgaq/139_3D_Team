
#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;

/* OutLine */
float gEdgeThreshold = 0.1f; // 엣지 감지 임계값
float2 gPixelOffset;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;	
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
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

struct PS_IN
{
	float4		vPosition	: SV_POSITION;
	float4		vNormal		: NORMAL;
	float2		vTexcoord	: TEXCOORD0;
	float4		vWorldPos	: TEXCOORD1;
	float4		vProjPos	: TEXCOORD2;
	float4		vTangent	: TANGENT;
	float4		vBinormal	: BINORMAL;
};

struct PS_OUT 
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};

/* ------------------- Base Pixel Shader (0) -------------------*/

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

	vNormal = mul(vNormal, WorldMatrix);


	if (vMtrlDiffuse.a < 0.3f)
		discard;
	
	Out.vDiffuse = vMtrlDiffuse;	
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);

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

/* ------------------- OutLine Pixel Shader(3) -------------------*/

PS_OUT PS_MAIN_OUTLINE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3x3 K = { -2, -1, 0, -1, 0, 1, 0, 1, 2 };

    // 1/Width, 1/Height
    float2 PixelOffset = (1 / 1024, 1 / 1024);
	
    float Lx = 0;
    float Ly = 0;
	
    float res = 0;
	
    for( int y = -1; y <= 1; ++y)
	{
        for (int x = -1; x <= 1; ++x)
        {
            float2 offset = float2(x, y) * PixelOffset;
            float3 tex = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + offset).rgb;
            float Luminance = dot(tex, float3(0.3, 0.59, 0.11));
			
            res += Luminance * K[y + 1][x + 1];
        }
    }
	
    res += 0.5f;
	
    Out.vDiffuse = float4(res.xxx, 1);
	
    return Out;
	
 //   float mask[9] = { -1, -1, -1, -1, 8, -1, -1, -1, -1 };
 //   float Coord[3] = { -1, 0, +1 };
 //   float divider = 0.2f;
	
 //   float4 vColor = 0;
 //   float3 grayScale = float3(0.3, 0.59, 0.11);
	
 //   float MAX_CX = 3.f;
 //   float MAP_CY = 3.f;
	
 //   for (int i = 0; i < 9; ++i)
 //   {
 //       vColor += mask[i] * (g_NormalTexture.Sample(LinearSampler, In.vTexcoord + float2(Coord[i % 3] / MAX_CX, Coord[i / 3] / MAP_CY)));
 //   }
	
 //   float gray = 1.0f - dot(vColor.rgb, grayScale);
    
	//float laplacian = dot(vColor.rgb, float3(1, 1, 1)) - 8 * gray;
 //   gray += laplacian;

 //   Out.vDiffuse = float4(gray, gray, gray, 1.0f) / divider;
	
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
    pass OutLine // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
    }
}
