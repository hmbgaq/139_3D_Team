#include "Shader_Defines.hlsli"

/* Origin */
matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D   g_ProcessingTarget;

bool g_bHDR_Active;
/* HDR - off일수 있으므로 초기값 줘야함 */ 
float       g_fGrayScale = 1.f;
float       g_fContrastValue = 1.f;
float       g_fSaturation = 1.f; /*  값을 0 ~ 1로 고정해주는 Clamp 역할*/ 
float       g_fPadding;

struct VS_IN
{

    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

/* =================== Base Vertex Shader =================== */
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

/* =================== Base Pixel Shader (0) =================== */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* ------------------- 0 - Origin Pixel Shader -------------------*/

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vProcess = g_ProcessingTarget.Sample(PointSampler, In.vTexcoord);
    
    Out.vColor = vProcess;
    
    return Out;
}


/* ------------------- 1 - HDR Pixel Shader -------------------*/

float3 reinhard(float3 hdr)
{
    return hdr / (1.0 + hdr);
}

PS_OUT PS_MAIN_HDR(PS_IN In)
{
    /* Reinhard TMO */ 
    PS_OUT Out = (PS_OUT) 0;
    
    float3 vColor = g_ProcessingTarget.Sample(LinearSampler, In.vTexcoord).xyz;
	
    vColor = reinhard(vColor);

    Out.vColor = float4(vColor, 1.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Origin // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass HDR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HDR();
    }
}
