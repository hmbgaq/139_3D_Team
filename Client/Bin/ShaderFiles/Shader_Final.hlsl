#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_FinalTarget; /* 기존 Deferred로 넘어온색 */ 
Texture2D g_Final_Effect;
Texture2D g_Diffuse_UITarget;

/* 명도 채도 관리 */
float g_brightness = 1.f; // 명도 
float g_saturation = 1.f; // 채도 

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

VS_OUT VS_MAIN_FINAL(VS_IN In)
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

float3 AdjustHue(float3 hsv, float hueShift)
{
    // hueShift는 각도로 표현되며, 이를 [0, 360] 범위로 조절
    hueShift = frac(hueShift / 360.0);

    // 이미지의 색조를 조절
    hsv.x += hueShift;
    hsv.x = frac(hsv.x); // 0~1 범위로 유지

    return hsv;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
   // float4 vUI = g_Diffuse_UITexture.Sample(LinearSampler, In.vTexcoord);
    float4 vEffect = g_Final_Effect.Sample(LinearSampler, In.vTexcoord);
    
    if (vEffect.a == 0.0f)
    {
        float4 originalColor = g_FinalTarget.Sample(LinearSampler, In.vTexcoord);
            if (originalColor.a == 0.f)
               discard;
    
        // 명도 적용
        float3 BrightColor = originalColor.rgb * g_brightness;
    
        float value = BrightColor.r + (1.0 - g_brightness);
    
        // 채도 적용
        BrightColor = lerp(float3(value, value, value), BrightColor, g_saturation);
        Out.vColor = float4(BrightColor, originalColor.a);
    }

    return Out;
}

technique11 DefaultTechnique
{
    pass Final // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_FINAL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

}