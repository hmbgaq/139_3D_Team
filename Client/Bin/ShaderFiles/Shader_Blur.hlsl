#include "Shader_Defines.hlsli"

/* ------------------- Common Vertex Shader  -------------------*/
// 분리형 가우시안블러
// 아래의 fWeight가 가우시안블러의 가중치인데 밖에서 값던지나 여기서 적용하나 그게그거라 걍넣음 
// 순서는 다운샘플링 -> 수평블러 -> 수직블러 -> 원래해상도로 업샘플링이 순서임. 
/* -------------------------------------------------------------*/

matrix      g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D   g_BlurTarget;

int         g_iBlurSamplers = 15;
float       g_fBlurRange = 0.09f;

float2      g_WinSize = float2(1920.f, 1080.f);

float       g_fWeight_low[3] = { 0.025f, 0.95f, 0.025f };
float       g_fWeight_quarter[7] = { 0.2f, 0.5f, 0.8f, 1.f, 0.8f, 0.5f, 0.2f };
float       g_fWeight_middle[11] = { 0.1f, 0.2f, 0.4f, 0.6f, 0.8f, 1.f, 0.8f, 0.6f, 0.4f, 0.2f, 0.1f };
float       g_fWeight_high[19] = { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.f, 0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f };

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

/* ------------------- Common Vertex Shader  -------------------*/

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

/* ------------------- 0 - Pixel Shader / BLUR_DOWN -------------------*/
PS_OUT PS_BLUR_DOWN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_BlurTarget.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- 1 - Pixel Shader / BLUR_HORIZON_LOW -------------------*/
PS_OUT PS_BLUR_H_LOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
    float fTotal = 0.f;

    // 텍셀의 크기 -> 인접한 픽셀의 UV좌표를 계산할 수 있다. 
    float texelsize = 1.f / (g_WinSize.x);
    
    /* -4 ~ 0 ~ 4 까지 구하듯이 인접한 픽셀의 UV좌표를 구한다. : low : -1 0 1  */ 
    for (int i = -1; 2 > i; i++)
    {
        vColor += g_fWeight_low[i + 1] * g_BlurTarget.Sample(LinearSampler, In.vTexcoord + float2(1.f / (g_WinSize.x / 2.f) * i, 0.f));
        fTotal += g_fWeight_low[i + 1];
    }

    Out.vColor = vColor / fTotal;
    
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- 2 - Pixel Shader / BLUR_HORIZON_QUARTER,	 -------------------*/
PS_OUT PS_BLUR_H_QUARTER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
    float fTotal = 0.f;

// Add padding to the image edges
   //float2 texSize = g_BlurTarget.GetDimensions(0, texSize.x, texSize.y, 0); // 텍스처의 너비와 높이를 가져와 저장 
   //float2 texelSize = 1.0 / texSize; // 텍셀의 크기 계산 
   //float2 padding = texelSize * 0.5; // 이미지 가장자리에 추가할 패딩의 크기를 계산 
   //In.vTexcoord = clamp(In.vTexcoord, padding, 1.0 - padding);
    
    for (int i = -3; i < 4; i++)
    {
        vColor += g_fWeight_quarter[i + 3] * g_BlurTarget.Sample(LinearSampler, In.vTexcoord + float2(1.f / (g_WinSize.x / 2.f) * i, 0.f));
        fTotal += g_fWeight_quarter[i + 3];
    }

    Out.vColor = vColor / fTotal;
    
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- 3 - Pixel Shader / BLUR_HORIZON_MIDDLE,	 -------------------*/
PS_OUT PS_BLUR_H_MIDDLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    float fTotal = 0.f;

    for (int i = -5; i < 6 ; i++)
    {
        vColor += g_fWeight_middle[i + 5] * g_BlurTarget.Sample(LinearSampler, In.vTexcoord + float2(1.f / (g_WinSize.x / 2.f) * i, 0.f));
        fTotal += g_fWeight_middle[i + 5];
    }

    Out.vColor = vColor / fTotal;
    
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- 4 - Pixel Shader / BLUR_HORIZON_HIGH,	 -------------------*/
PS_OUT PS_BLUR_H_HIGH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    float fTotal = 0.f;

    for (int i = -9; 10 > i; i++)
    {
        vColor += g_fWeight_high[i + 9] * g_BlurTarget.Sample(LinearSampler, In.vTexcoord + float2(1.f / (g_WinSize.x / 2.f) * i, 0.f));
        fTotal += g_fWeight_high[i + 9];
    }

    Out.vColor = vColor / fTotal;
    
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- 5 - Pixel Shader / BLUR_VERTICAL_LOW,	 -------------------*/
PS_OUT PS_BLUR_V_LOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float fTotal = 0.f;

    for (int i = -1; 2 > i; i++)
    {
        vColor += g_fWeight_low[i + 1] * g_BlurTarget.Sample(LinearSampler, In.vTexcoord + float2(0, 1.f / (g_WinSize.y / 2.f) * i));
        fTotal += g_fWeight_low[i + 1];
    }

    Out.vColor = vColor / fTotal;
    
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- 6 - Pixel Shader / BLUR_VERTICAL_QUARTER,	 -------------------*/
PS_OUT PS_BLUR_V_QUARTER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float fTotal = 0.f;
    
    // Add padding to the image edges
   // float2 texSize = g_BlurTarget.GetDimensions(0, texSize.x, texSize.y, 0 );
   // float2 texelSize = 1.0 / texSize;
   // float2 padding = texelSize * 0.5; // Half a texel
   // In.vTexcoord = clamp(In.vTexcoord, padding, 1.0 - padding);
    
    for (int i = -3; 4 > i; i++)
    {
        vColor += g_fWeight_quarter[i + 3] * g_BlurTarget.Sample(LinearSampler, In.vTexcoord + float2(0, 1.f / (g_WinSize.y / 2.f) * i));
        fTotal += g_fWeight_quarter[i + 3];
    }

    Out.vColor = vColor / fTotal;
    
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- 7 - Pixel Shader / BLUR_VERTICAL_MIDDLE,	 -------------------*/
PS_OUT PS_BLUR_V_MIDDLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float fTotal = 0.f;

    for (int i = -5; 6 > i; i++)
    {
        vColor += g_fWeight_middle[i + 5] * g_BlurTarget.Sample(LinearSampler, In.vTexcoord + float2(0, 1.f / (g_WinSize.y / 2.f) * i));
        fTotal += g_fWeight_middle[i + 5];
    }

    Out.vColor = vColor / fTotal;
    
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- 8 - Pixel Shader / BLUR_VERTICAL_HIGH,	 -------------------*/
PS_OUT PS_BLUR_V_HIGH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float fTotal = 0.f;

    for (int i = -9; i < 10 ; i++)
    {
        vColor += g_fWeight_high[i + 9] * g_BlurTarget.Sample(LinearSampler, In.vTexcoord + float2(0, 1.f / (g_WinSize.y / 2.f) * i));
        fTotal += g_fWeight_high[i + 9];
    }

    Out.vColor = vColor / fTotal;
    
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- 9, 10 - Pixel Shader / BLUR_UP_ADD, BLUR_UP_MAX	 -------------------*/
PS_OUT PS_BLUR_UP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_BlurTarget.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- Technique -------------------*/ 

technique11 DefaultTechnique
{
    pass Blur_Down // 0 BLUR_DOWN, 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Max, float4(0.0f, 0.0f, 0.0f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_DOWN();
    }

    pass Horizon_Low // 1 BLUR_HORIZON_LOW
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Max, float4(0.0f, 0.0f, 0.0f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_H_LOW();
    }

    pass Horizon_Quarter // 2 BLUR_HORIZON_QUARTER
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Max, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_H_QUARTER();
    }

    pass Horizon_Middle // 3 BLUR_HORIZON_MIDDLE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Max, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_H_MIDDLE();
    }

    pass Horizon_High // 4 BLUR_HORIZON_HIGH
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Max, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_H_HIGH();
    }

    pass Vertical_Low // 5 VERTICAL_LOW
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Max, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_V_LOW();
    }

    pass Vertical_Quarter // 6 BLUR_VERTICAL_QUARTER,	
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Max, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_V_QUARTER();
    }

    pass Vertical_Middle // 7 BLUR_VERTICAL_MIDDLE,	
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Max, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_V_MIDDLE();
    }

    pass Vertical_High // 8 BLUR_VERTICAL_HIGH,
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Max, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_V_HIGH();
    }

    pass Blur_UpAdd // 9 BLUR_UP_ADD, 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_UP();
    }

    pass Blur_UpMax // 10 BLUR_UP_MAX,
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Max, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_UP();
    }

}