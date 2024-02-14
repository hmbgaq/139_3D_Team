#include "Shader_Defines.hlsli"

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4      g_vLineColor;
Texture2D   g_NormalTarget;
float       g_fCoord[3] = { -1, 0, +1 };
float       g_fDivier = 1;
float       g_fBias = 0.5f;

float g_fMask[9] =
{
    -1, -1, -1,
	-1,  8, -1,
	-1, -1, -1
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float fFogFactor : FOG;
};

/* ------------------- Base Vertex Shader -------------------*/

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* ------------------- Base Pixel Shader (0) -------------------*/

PS_OUT PS_OUTLINE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 Color = 0;
	
    for (int i = 0; i < 9; i++)
    {
        vector vNormalDesc = g_NormalTarget.Sample(PointSampler, In.vTexcoord + float2(g_fCoord[i % 3.f] / 512.f, g_fCoord[i % 3.f] / 512.f) * g_fBias);
        vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
        Color += g_fMask[i] * vNormal;
    }
	
    float gray = 1 - (Color.r * 0.3 + Color.g * 0.59 + Color.b * 0.11);
	
    float4 lineColor = g_vLineColor;
    Out.vColor = float4(lerp(lineColor.rgb, float3(gray, gray, gray), saturate(gray)), 1.0) / g_fDivier;

    return Out;
}


/* ------------------- Technique -------------------*/ 

technique11 DefaultTechnique
{
    pass OutLine // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE();
    }
}
