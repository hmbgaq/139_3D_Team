#include "Shader_Defines.hlsli"

/* =================== Struct =================== */
struct radial
{
    bool    bRadial_Active;
    float   fRadial_Quality;
    float   fRadial_Power;
};

struct DOF
{
    bool    bDOF_Active;
    float   fFocusDistance;
    float   fFocusRange;
};

/* =================== Variable =================== */
// Origin
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
float4 g_vCamPosition;
float  g_fCamFar;
Texture2D g_ProcessingTarget;

// HDR 
bool g_bHDR_Active;
float g_max_white;
float g_change_luminance;

// Radial Blur 
radial g_Radial_Blur;

// DOF
Texture2D   g_DepthTarget;
Texture2D   g_BlurTarget;
DOF         g_DOF;

// EffectMix
Texture2D g_Deferred_Target;
Texture2D g_RimBlur_Target;
Texture2D g_Effect_Target;
Texture2D g_EffectBlur_Target;

/* =================== Function =================== */

float3 reinhard_extended(float3 v, float max_white)
{
    /*  reinhard_extended */ 
    float Value = max_white * max_white;
    
    float3 numerator = v * (1.0f + (v / float3(Value, Value, Value)));
   
    return numerator / (1.0f + v);
}

float luminance(float3 v)
{
    /* CommonFunction */ 
    return dot(v, float3(0.2126f, 0.7152f, 0.0722f));
}

float3 change_luminance(float3 Color, float global_variable)
{
    /* Luminance */ 
    float Temp = luminance(Color);
    return Color * (global_variable / Temp);
}

float3 reinhard_extended_luminance(float3 Color, float global_variable)
{
    /* Extended Reinhard (Luminance Tone Map) */ 
    float Old_Color = luminance(Color);
    float numerator = Old_Color * (1.0f + (Old_Color / (global_variable * global_variable)));
    float Temp_New = numerator / (1.0f + Old_Color);
    return change_luminance(Color, Temp_New);
}

float3 reinhard_jodie(float3 Color)
{
    float l = luminance(Color);
    float3 RJ_Map = Color / (1.0f + Color);
    return lerp(Color / (1.0f + l), RJ_Map, RJ_Map);
}

float3 uncharted2_tonemap_partial(float3 Color)
{
    float A = 0.15f;
    float B = 0.50f;
    float C = 0.10f;
    float D = 0.20f;
    float E = 0.02f;
    float F = 0.30f;
    return ((Color * (A * Color + C * B) + D * E) / (Color * (A * Color + B) + D * F)) - E / F;
}

float3 uncharted2_filmic(float3 Color)
{
    float exposure_bias = 2.0f;
    float3 curr = uncharted2_tonemap_partial(Color * exposure_bias);

    float3 W = float3(11.2f, 11.2f, 11.2f);
    float3 white_scale = float3(1.0f, 1.0f, 1.0f) / uncharted2_tonemap_partial(W);
    return curr * white_scale;
}

float3 mul(float3x3 m, float3 v)
{
    float x = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
    float y = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
    float z = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];
    
    return float3(x, y, z);
}

float3 rtt_and_odt_fit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float3 aces_fitted(float3 Color)
{
    float3x3 aces_input_matrix = (0.59719f, 0.35458f, 0.04823f, 0.07600f, 0.90834f, 0.01566f, 0.02840f, 0.13383f, 0.83777f);
    float3x3 aces_output_matrix = (1.60475f, -0.53108f, -0.07367f, -0.10208f, 1.10813f, -0.00605f, -0.00327f, -0.07276f, 1.07602f);
    
    Color = mul(aces_input_matrix, Color);
    Color = rtt_and_odt_fit(Color);
    return mul(aces_output_matrix, Color);
}

/* =================== VS / PS =================== */

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

/* ------------------- 0 - Origin Pixel Shader -------------------*/

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vProcess = g_ProcessingTarget.Sample(PointSampler, In.vTexcoord);
    
    Out.vColor = vProcess;
    
    return Out;
}

/* ------------------- 1 - HDR Pixel Shader -------------------*/
PS_OUT PS_MAIN_HDR(PS_IN In)
{
    /* Reinhard TMO */ 
    PS_OUT Out = (PS_OUT) 0;
    
    float3 vColor = g_ProcessingTarget.Sample(LinearSampler, In.vTexcoord).xyz;
	
    //vColor = reinhard_extended(vColor, g_max_white);
    
    //vColor = change_luminance(vColor,g_max_white) ;
    
    vColor = reinhard_extended_luminance(vColor, g_max_white);
    
    //vColor = reinhard_jodie(vColor);
    
   // vColor = uncharted2_filmic(vColor);
    
    //vColor = aces_fitted(vColor);
    
    // vColor = camera_tonemap(vColor, g_max_white); /* camera_irradiance 데이터값이 없어서불가 */ 
    
    Out.vColor = float4(vColor, 1.f);
    
    return Out;
}

/* ------------------- 2 - Radial Blur Pixel Shader -------------------*/
PS_OUT PS_MAIN_RADIAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;


    float4 colour = { 0.f, 0.f, 0.f, 0.f };
    float v = 0.f;

    for (float i = 0.0f; i < 1.0f; i += (1 / g_Radial_Blur.fRadial_Quality))
    {
        v = 0.9 + i * g_Radial_Blur.fRadial_Power;
        colour += g_ProcessingTarget.Sample(PointSampler, In.vTexcoord * v + 0.5f - 0.5f * v);
    }

    colour /= g_Radial_Blur.fRadial_Quality;
    colour.a = 1.f;
    
    Out.vColor = colour;
    return Out;
    
}

/* ------------------- 3 -DOF Shader -------------------*/

PS_OUT PS_MAIN_DOF (PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDepth = g_DepthTarget.Sample(LinearSampler, In.vTexcoord);    
    vector vTarget = g_ProcessingTarget.Sample(LinearSampler, In.vTexcoord);
    vector vBlur = g_BlurTarget.Sample(LinearSampler, In.vTexcoord);
    
    float fViewZ = vDepth.y * g_fCamFar;
    
    if (g_DOF.fFocusDistance - g_DOF.fFocusRange > fViewZ) /* 초점거리 앞 */ 
    {
        Out.vColor = vBlur;
    }
    else if (g_DOF.fFocusDistance + g_DOF.fFocusRange < fViewZ) /* 초첨거리 뒤 */
    {
        Out.vColor = vBlur;
    }
    else /* 정상출력할곳 */ 
        Out.vColor = vTarget;

    return Out;
}

/* ------------------- 4 -EffectMix Shader -------------------*/

PS_OUT PS_MAIN_EFFECTMIX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    vector Deferred = g_Deferred_Target.Sample(LinearSampler, In.vTexcoord);
    vector Object_Blur = g_RimBlur_Target.Sample(LinearSampler, In.vTexcoord);
    
    vector Effect = g_Effect_Target.Sample(LinearSampler, In.vTexcoord);
  //  vector Effect_Blur = g_EffectBlur_Target.Sample(LinearSampler, In.vTexcoord);
    
    // Out.vColor = Deferred + Object_Blur + Effect + Effect_Blur;
    
    
    /* 여기 건들면 될거같은데 왜지 뭐지 왜안되지 ! */
    
    Out.vColor = Effect;
    // +Effect_Blur;
    
    if(Out.vColor.a == 0)
        Out.vColor =  Deferred + Object_Blur;
  
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- Technique  -------------------*/

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

    pass HDR // 1
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

    pass RADIAL // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RADIAL();

    }

    pass DOF // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL ;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DOF();
    }

    pass EffectMix // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECTMIX();
    }
}