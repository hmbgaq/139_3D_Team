#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Final_Deferred_Target;
Texture2D g_Final_Effect_Target;
Texture2D g_PriorityTarget;
Texture2D g_Final_UI_Target;

Texture2D g_FinalTarget;
Texture2D g_BlendMixTarget;
Texture2D g_DebugTarget;
Texture2D g_UI_Target;
Texture2D g_Effect_Target;


/* ------------------ Struct  ------------------ */ 

struct HSV_DESC
{
    bool bScreen_Active;
    float fFinal_Saturation; // 채도 
    float fFinal_Brightness; // 명도 

};

HSV_DESC g_HSV_DESC;
/* ------------------ Function ------------------ */ 

// RGB를 HSV로 변환하는 함수
float3 rgb2hsv(float3 rgb)
{
    float r = rgb.r;
    float g = rgb.g;
    float b = rgb.b;

    float maxVal = max(max(r, g), b);
    float minVal = min(min(r, g), b);

    float delta = maxVal - minVal;

    float h = 0;
    float s = (maxVal > 0) ? (delta / maxVal) : 0;
    float v = maxVal;

    if (delta > 0)
    {
        if (maxVal == r)
            h = 60 * ((g - b) / delta);
        else if (maxVal == g)
            h = 120 + 60 * ((b - r) / delta);
        else
            h = 240 + 60 * ((r - g) / delta);

        if (h < 0)
            h += 360;
    }

    return float3(h / 360, s, v);
}

// HSV를 RGB로 변환하는 함수
float3 hsv2rgb(float3 hsv)
{
    float h = hsv.x * 360;
    float s = hsv.y;
    float v = hsv.z;

    float c = v * s;
    float x = c * (1 - abs(fmod(h / 60.0, 2) - 1));
    float m = v - c;

    float3 rgb;

    if (h < 60)
        rgb = float3(c, x, 0);
    else if (h < 120)
        rgb = float3(x, c, 0);
    else if (h < 180)
        rgb = float3(0, c, x);
    else if (h < 240)
        rgb = float3(0, x, c);
    else if (h < 300)
        rgb = float3(x, 0, c);
    else
        rgb = float3(c, 0, x);

    return rgb + m;
}
/* ------------------ VS / PS ------------------ */ 

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


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* ------------------ ------------------ */ 
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

/* ------------------ 0 - Default ------------------ */
PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_FinalTarget.Sample(PointSampler, In.vTexcoord);

    return Out;    
}

/* ------------------ 1 - HSV ------------------ */
PS_OUT PS_MAIN_HSV(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 텍스처에서 원본 색상을 샘플링
    float4 OriginalColor = g_FinalTarget.Sample(LinearSampler, In.vTexcoord);
    
    if (true == g_HSV_DESC.bScreen_Active)
    {
        // 원본 색상을 HSV 모델로 변환
        float3 hsv = rgb2hsv(OriginalColor.rgb);
        
        hsv.z *= g_HSV_DESC.fFinal_Brightness; // 명도
        hsv.y *= g_HSV_DESC.fFinal_Saturation; // 채도
        
        // 최종 색상을 출력에 할당
        Out.vColor = float4(hsv2rgb(hsv), OriginalColor.a);
    }
    else
        Out.vColor = OriginalColor;
    
    return Out;
}
    
/* ------------------ 2 - MIX ------------------ */
PS_OUT PS_MAIN_MIX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_BlendMixTarget.Sample(PointSampler, In.vTexcoord);

    return Out;
}
/* ------------------ 3 - Final ------------------ */
PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDebug = g_DebugTarget.Sample(LinearSampler, In.vTexcoord);;
    vector vFinal = g_FinalTarget.Sample(LinearSampler, In.vTexcoord);
    vector vUI = g_UI_Target.Sample(LinearSampler, In.vTexcoord);
   
    Out.vColor = vUI;
    
    if (Out.vColor.a == 0)
    {
        Out.vColor = vFinal + vDebug;
        
        if(Out.vColor.a == 0)
           discard;
    }
    return Out;
}
/* ------------------ 4 - Effect Blend ------------------ */
PS_OUT PS_MAIN_BLENDEFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vOrigin = g_FinalTarget.Sample(LinearSampler, In.vTexcoord); /* Target_Deferred */
    vector vEffect = g_Effect_Target.Sample(LinearSampler, In.vTexcoord); /* Target_Deferred */
    
    Out.vColor = vOrigin + vEffect;
    
    return Out;
}
/* ------------------ Technique ------------------ */

technique11 DefaultTechnique
{
    pass Default // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
    }

    pass HSV // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HSV();
    }

    pass MIX // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend_Add, float4(0.0f, 0.0f, 0.0f, 1.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MIX();
    }

    pass Final // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();

    }

    pass MIX_EFFECT // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLENDEFFECT();
    }
}