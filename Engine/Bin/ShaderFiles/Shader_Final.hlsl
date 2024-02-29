#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Final_Deferred_Target;
Texture2D g_Final_Effect_Target;
Texture2D g_PriorityTarget;
Texture2D g_Final_UI_Target;

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
    float4 vEffect = g_Final_Effect_Target.Sample(LinearSampler, In.vTexcoord);
    float4 originalColor = g_Final_Deferred_Target.Sample(LinearSampler, In.vTexcoord);
    float4 vPriority = g_PriorityTarget.Sample(LinearSampler, In.vTexcoord);
    
    float4 vResult = vEffect + originalColor;
    
    if (vResult.a == 0)
    {
        Out.vColor = g_PriorityTarget.Sample(LinearSampler, In.vTexcoord);
    }
    else
    {
        Out.vColor = vResult;
    }
    
    if (Out.vColor.a == 0) // 백버퍼 blue 나오게 
        discard;

    return Out;    
}

  
    
technique11 DefaultTechnique
{
    pass Final // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_FINAL();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

}