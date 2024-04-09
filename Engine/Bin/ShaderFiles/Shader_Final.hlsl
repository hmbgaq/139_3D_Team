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
Texture2D g_RimBlur_Target;
Texture2D g_OutLine_Target;
Texture2D g_Independent_Target;


/* ------------------ Value ------------------ */ 
float g_GreyPower = 0.11f;
float g_SepiaPower = 0.58f;

/* ------------------ Define ------------------ */ 
#define ColorTone float3(1.40, 1.10, 0.90) //[0.00 to 2.55, 0.00 to 2.55, 0.00 to 2.55] What color to tint the image
#define Monochrome_conversion_values float3(0.18,0.41,0.41) //[0.00 to 1.00] Percentage of RGB to include (should sum up to 1.00)

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

float4 Sepia(float4 colorInput)
{
    float3 sepia = colorInput.rgb;
	
	// calculating amounts of input, grey and sepia colors to blend and combine
    float grey = dot(sepia, float3(0.2126, 0.7152, 0.0722));
	
    sepia *= ColorTone;
	
    float3 blend2 = (grey * g_GreyPower) + (colorInput.rgb / (g_GreyPower + 1));

    colorInput.rgb = lerp(blend2, sepia, g_SepiaPower);
	
	// returning the final color
    return colorInput;
}

float4 MonochromePass(float4 colorInput)
{
  //calculate monochrome
    colorInput.rgb = dot(Monochrome_conversion_values, colorInput.rgb);
	
  //Return the result
    return saturate(colorInput);
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

float2 center = { 0.5f, 0.5f };
float frequency = 1;
float offset = 10.f;
float dpi = 96;

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDebug = g_DebugTarget.Sample(LinearSampler, In.vTexcoord);;
    vector vFinal = g_FinalTarget.Sample(LinearSampler, In.vTexcoord);
    vector vUI = g_UI_Target.Sample(LinearSampler, In.vTexcoord);
    vector vRimBloom = g_RimBlur_Target.Sample(LinearSampler, In.vTexcoord);
    vector vOutLine = g_OutLine_Target.Sample(LinearSampler, In.vTexcoord);
    vector vIndep = g_Independent_Target.Sample(LinearSampler, In.vTexcoord);
    
    float4 MainObject = vFinal + vDebug + vRimBloom;
    
    Out.vColor = vUI;
       
   if (Out.vColor.a == 0)
        Out.vColor = vIndep ;
    
    if(Out.vColor.a == 0)
        Out.vColor = vOutLine;
    
    if (Out.vColor.a == 0)
        Out.vColor = MainObject;
    
    if(Out.vColor.a == 0)
        discard;
       
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
/* ------------------ 5 - Sephia ------------------ */

PS_OUT PS_MAIN_FINAL_SEPHIA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDebug = g_DebugTarget.Sample(LinearSampler, In.vTexcoord);;
    vector vFinal = g_FinalTarget.Sample(LinearSampler, In.vTexcoord);
    vector vUI = g_UI_Target.Sample(LinearSampler, In.vTexcoord);
    vector vRimBloom = g_RimBlur_Target.Sample(LinearSampler, In.vTexcoord);
    vector vOutLine = g_OutLine_Target.Sample(LinearSampler, In.vTexcoord);
    vector vIndep = g_Independent_Target.Sample(LinearSampler, In.vTexcoord);
    float4 MainObject = vFinal + vDebug + vRimBloom + vOutLine;
    
    MainObject = Sepia(MainObject);
    vIndep = Sepia(vIndep);
    
    Out.vColor = vUI;
   
    if (Out.vColor.a == 0)
        Out.vColor = vIndep;
    
    if (Out.vColor.a == 0)
        Out.vColor = MainObject;
       
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- 6 - GrayScale ------------------ */
PS_OUT PS_MAIN_FINAL_GRAY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDebug = g_DebugTarget.Sample(LinearSampler, In.vTexcoord);;
    vector vFinal = g_FinalTarget.Sample(LinearSampler, In.vTexcoord);
    vector vUI = g_UI_Target.Sample(LinearSampler, In.vTexcoord);
    vector vRimBloom = g_RimBlur_Target.Sample(LinearSampler, In.vTexcoord);
    vector vOutLine = g_OutLine_Target.Sample(LinearSampler, In.vTexcoord);
    vector vIndep = g_Independent_Target.Sample(LinearSampler, In.vTexcoord);
    float4 MainObject = vFinal + vDebug + vRimBloom + vOutLine;
    
    MainObject = MonochromePass(MainObject);
    vIndep = MonochromePass(vIndep);
    
    Out.vColor = vUI;
    
    if (Out.vColor.a == 0)
        Out.vColor = vIndep;
    
    if (Out.vColor.a == 0)
        Out.vColor = MainObject;
       
    if (Out.vColor.a == 0)
        discard;
    
    
    return Out;
}

/* ------------------- 6 - GrayScale ------------------ */
PS_OUT PS_MAIN_TEST(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector Test = g_FinalTarget.Sample(LinearSampler, In.vTexcoord);
    
    if (Test.a == 0)
        discard;
    
    Out.vColor = Test;
    
    return Out;
    
}

/*=============================================================
 
                         Technique 
                                
==============================================================*/

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

    pass FINAL_SEPHIA // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL_SEPHIA();
    }

    pass FINAL_GRAY // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL_GRAY();
    }

    pass TEST
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TEST();
    }
}