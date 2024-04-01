#include "Shader_Defines.hlsli"

/*=============================================================
 
                             Global  
                                
==============================================================*/

#define CoefLuma float3(0.2126, 0.7152, 0.0722) 

/*=============================================================
 
                             Struct 
                                
==============================================================*/
struct radial
{
    float   fRadial_Quality;
    float   fRadial_Power;
    bool    bRadial_Active;
};

struct DOF
{
    float   DOF_Distance;
    bool    bDOF_Active;
};

struct VIGNETTE_DESC
{
    float   fVignetteRatio; //[0.15 to 6.00]  Sets a width to height ratio. 1.00 (1/1) is perfectly round, while 1.60 (16/10) is 60 % wider than it's high.
    float   fVignetteRadius; //[-1.00 to 3.00] lower values = stronger radial effect from center
    float   fVignetteAmount; //[-2.00 to 1.00] Strength of black. -2.00 = Max Black, 1.00 = Max White.
    float   fVignetteSlope; //[2 to 16] How far away from the center the change should start to really grow strong (odd numbers cause a larger fps drop than even numbers)
    float   fVignetteCenter_X;
    float   fVignetteCenter_Y;
    bool    bVignette_Active;
};

struct SSR_DESC
{
    float fRayStep; // 0.005
    float fStepCnt; // 75
    bool bSSR_Active;
};

struct CHROMA_DESC
{
    float fcaAmount;
    bool bChroma_Active;
};

struct MOTIONBLUR_DESC
{
    bool bMotionblur_Active;
    float fIntensity;
};

struct LUMA_DESC
{
    float fsharp_strength;
    float fsharp_clamp;
    float foffset_bias;
    bool bLumaSharpen_Active;
};
/*=============================================================
 
                             Variable 
                                
==============================================================*/
// Origin
float4x4    g_WorldMatrix;
float4x4    g_ViewMatrix;
float4x4    g_ProjMatrix;
float4x4    g_ViewMatrixInv;
float4x4    g_ProjMatrixInv;
float4x4    g_PreCamViewMatrix;
float4x4    g_CamViewMatrix;
float4x4    g_CamProjMatrix;
float4      g_vCamPosition;
float       g_fCamFar;
float       g_fCamNear;
Texture2D   g_ProcessingTarget;

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
Texture2D g_RimBlur_Target;
Texture2D g_Effect_Target;
Texture2D g_EffectBlur_Target;
Texture2D g_Effect_Solid;
Texture2D g_Distortion_Target;

// EffectDistortion
Texture2D g_Deferred_Target;
Texture2D g_Effect_DistortionTarget;

// Vignette
VIGNETTE_DESC g_Vignette_desc;

// SSR 
static const int SSR_MAX_STEPS = 16;
static const int SSR_BINARY_SEARCH_STEPS = 16;
Texture2D g_NormalTarget;
Texture2D g_ORMTarget;
SSR_DESC g_SSR_Desc;

// Chroma
CHROMA_DESC g_Chroma_Desc;

// Ice
Texture2D g_Ice_Target;

// MotionBLur
MOTIONBLUR_DESC g_MotionBlur;

// LumaSharpen
LUMA_DESC g_Luma_Desc;
/*=============================================================
 
                             Function 
                                
==============================================================*/

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

float4 vignette(float4 OriginColor, float2 texUV)
{
    // =================== Type 1 ===================

	// Set the center
    float2 distance_xy = texUV - float2(g_Vignette_desc.fVignetteCenter_X, g_Vignette_desc.fVignetteCenter_Y);
    
    float2 PixelSize = float2(1.0f / 1280.0f, 1.0f / 720.0f);

	// Adjust the ratio
    distance_xy *= float2((PixelSize.y / PixelSize.x), g_Vignette_desc.fVignetteRatio);

	// Calculate the distance
    distance_xy /= g_Vignette_desc.fVignetteRadius;
    
    float distance = dot(distance_xy, distance_xy);

	// Apply the vignette
    OriginColor.rgb *= (1.0 + pow(distance, g_Vignette_desc.fVignetteSlope * 0.5) * g_Vignette_desc.fVignetteAmount); //pow - multiply

    return OriginColor;
}

static float3 GetViewSpacePosition(float2 texcoord, float depth)
{
    float4 clipSpaceLocation;
    clipSpaceLocation.xy = texcoord * 2.0f - 1.0f;
    clipSpaceLocation.y *= -1;
    clipSpaceLocation.z = depth;
    clipSpaceLocation.w = 1.0f;
    float4 homogenousLocation = mul(clipSpaceLocation, g_ProjMatrixInv);
    return homogenousLocation.xyz / homogenousLocation.w;
}

float4 SSRBinarySearch(float3 dir, inout float3 hitCoord)
{
    float depth;
    for (int i = 0; i < SSR_BINARY_SEARCH_STEPS; i++)
    {
        float4 projectedCoord = mul(float4(hitCoord, 1.0f), g_ProjMatrix);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

        depth = g_DepthTarget.SampleLevel(PointClampSampler, projectedCoord.xy, 0);
        float3 viewSpacePosition = GetViewSpacePosition(projectedCoord.xy, depth);
        float depthDifference = hitCoord.z - viewSpacePosition.z;

        if (depthDifference <= 0.0f)
            hitCoord += dir;

        dir *= 0.5f;
        hitCoord -= dir;
   }

    float4 projectedCoord = mul(float4(hitCoord, 1.0f), g_ProjMatrix);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    
    depth = g_DepthTarget.SampleLevel(PointClampSampler, projectedCoord.xy, 0);
    float3 viewSpacePosition = GetViewSpacePosition(projectedCoord.xy, depth);
    float depthDifference = hitCoord.z - viewSpacePosition.z;

    return float4(projectedCoord.xy, depth, abs(depthDifference) < g_SSR_Desc.fStepCnt ? 1.0f : 0.0f);
//    return float4(projectedCoord.xy, depth, abs(depthDifference) < g_SSR_Desc.fRayHitThreshold ? 1.0f : 0.0f);
}

float4 SSRRayMarch(float3 dir, inout float3 hitCoord)
{
   float depth;
    
    for (int i = 0; i < SSR_MAX_STEPS; i++)
    {
        hitCoord += dir;
        float4 projectedCoord = mul(float4(hitCoord, 1.0f), g_ProjMatrix);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

        float4 DepthTexture = g_DepthTarget.SampleLevel(PointClampSampler, projectedCoord.xy, 0);
        float depth = DepthTexture.r;
        float3 viewSpacePosition = GetViewSpacePosition(projectedCoord.xy, depth);
        float depthDifference = hitCoord.z - viewSpacePosition.z;

		[branch]
        if (depthDifference > 0.0f)
        {
            return SSRBinarySearch(dir, hitCoord);
        }

        dir *= g_SSR_Desc.fRayStep;
    }
   return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

//Logical XOR - not used right now but it might be useful at a later time
float XOR(float xor_A, float xor_B)
{
    return saturate(dot(float4(-xor_A, -xor_A, xor_A, xor_B), float4(xor_B, xor_B, 1.0, 1.0))); // -2 * A * B + A + B
}


bool IsInsideScreen(float2 vCoord)
{
    return !(vCoord.x < 0 || vCoord.x > 1 || vCoord.y < 0 || vCoord.y > 1);
}


float BlurFactor(float depth, float4 DOF)
{
    float f0 = 1.0f - saturate((depth - DOF.x) / max(DOF.y - DOF.x, 0.01f));
    float f1 = saturate((depth - DOF.z) / max(DOF.w - DOF.z, 0.01f));
    float blur = saturate(f0 + f1);

    return blur;
}
float BlurFactor2(float depth, float2 DOF)
{
    return saturate((depth - DOF.x) / (DOF.y - DOF.x));
}
static float ConvertZToLinearDepth(float depth)
{
    float cameraNear = g_fCamNear;
    float cameraFar = g_fCamFar;
    return (cameraNear * cameraFar) / (cameraFar - depth * (cameraFar - cameraNear));
}
float3 DistanceDOF(float3 colorFocus, float3 colorBlurred, float depth)
{
    float blurFactor = BlurFactor(depth, g_DOF.DOF_Distance);
    return lerp(colorFocus, colorBlurred, blurFactor);
}

float4 LumaSharpenPass(float4 inputcolor, float2 texcoord)
{
    // -- Get the original pixel --
    float3 ori = g_ProcessingTarget.Sample(LinearSampler, texcoord).rgb;
   
    // -- Combining the strength and luma multipliers --
    float3 sharp_strength_luma = (CoefLuma * g_Luma_Desc.fsharp_strength); //I'll be combining even more multipliers with it later on
    
    float px = 1.f / 1280.f;
    float py = 1.f / 720.f;
    
    float3 blur_ori = g_ProcessingTarget.Sample(LinearSampler, texcoord + float2(px, -py) * 0.5 * g_Luma_Desc.foffset_bias).rgb; // South East
    blur_ori += g_ProcessingTarget.Sample(LinearSampler, texcoord + float2(-px, -py) * 0.5 * g_Luma_Desc.foffset_bias).rgb; // South West
    blur_ori += g_ProcessingTarget.Sample(LinearSampler, texcoord + float2(px, py) * 0.5 * g_Luma_Desc.foffset_bias).rgb; // North East
    blur_ori += g_ProcessingTarget.Sample(LinearSampler, texcoord + float2(-px, py) * 0.5 * g_Luma_Desc.foffset_bias).rgb; // North West

    blur_ori *= 0.25; // ( /= 4) Divide by the number of texture fetches
    
    /*-----------------------------------------------------------/
    /                            Sharpen                         /
    /-----------------------------------------------------------*/

    // -- Calculate the sharpening --
    float3 sharp = ori - blur_ori; //Subtracting the blurred image from the original image
    float4 sharp_strength_luma_clamp = float4(sharp_strength_luma * (0.5 / g_Luma_Desc.fsharp_clamp), 0.5); //Roll part of the clamp into the dot

    //sharp_luma = saturate((0.5 / sharp_clamp) * sharp_luma + 0.5); //scale up and clamp
    float sharp_luma = saturate(dot(float4(sharp, 1.0), sharp_strength_luma_clamp)); //Calculate the luma, adjust the strength, scale up and clamp
    sharp_luma = (g_Luma_Desc.fsharp_clamp * 2.0) * sharp_luma - g_Luma_Desc.fsharp_clamp; //scale down

    // -- Combining the values to get the final sharpened pixel	--
    //float4 done = ori + sharp_luma;    // Add the sharpening to the original.
    inputcolor.rgb = inputcolor.rgb + sharp_luma; // Add the sharpening to the input color.

    /*-----------------------------------------------------------/
    /                     Returning the output                   /
    /-----------------------------------------------------------*/
    
    //inputcolor.rgb = saturate(0.5 + (sharp_luma * 4)).rrr;

    return saturate(inputcolor);
}
/*=============================================================
 
                             IN/OUT  
                                
==============================================================*/

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

struct VS_OUT_SSR
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float4 vPositionClip : TEXCOORD1;
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

/*=============================================================
 
                         Vertex Shader 
                                
==============================================================*/
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    float4x4 matWV;
    float4x4 matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);


    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

/*=============================================================
 
                          Pixel Shader  
                                
==============================================================*/
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
    
    //depth = ConvertZToLinearDepth(depth);
    //color = float4(DistanceDOF(color.xyz, colorBlurred, depth), 1.0);
    //Out.vColor = color;
   
    float fViewZ = vDepth.y * g_fCamFar; /* 해당 픽셀이 카메라에서 얼마나 떨어져 있는지를 나타내는 월드 공간에서의 Z 값 */ 
    
    if (g_DOF.DOF_Distance > fViewZ) 
    {
        Out.vColor = vTarget;
    }
    else
        Out.vColor = vBlur;
    
    //else if (g_DOF.fFocusDistance + g_DOF.fFocusRange < fViewZ) /* 초첨거리 뒤 */
    //{
    //    Out.vColor = vBlur;
    //}
    
    //if (vDepth.a < 0.5)
    //    return vTarget;
    
    return Out;
}

/* ------------------- 4 -EffectMix Shader -------------------*/

PS_OUT PS_MAIN_EFFECTMIX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector Deferred = g_Deferred_Target.Sample(LinearSampler, In.vTexcoord);
   // vector Ice = g_Ice_Target.Sample(LinearSampler, In.vTexcoord);
    
    vector Effect = g_Effect_Target.Sample(LinearSampler, In.vTexcoord);
    vector Effect_Blur = g_EffectBlur_Target.Sample(LinearSampler, In.vTexcoord);
    vector Effect_Solid = g_Effect_Solid.Sample(LinearSampler, In.vTexcoord);
    vector Effect_Distortion = g_Distortion_Target.Sample(LinearSampler, In.vTexcoord);
    
    
    Out.vColor = Effect_Solid;
    
    if (Out.vColor.a == 0) 
        Out.vColor += Effect_Distortion;
    
    if (Out.vColor.a == 0) 
        Out.vColor += Deferred + Effect + Effect_Blur;
       // Out.vColor += Deferred + Effect + Effect_Blur + Ice;
    
    ////if(Out.vColor.a == 0) /* 그뒤에 디퍼드 + 디퍼드 블러 같이 그린다. */ 
    //    //Out.vColor += Effect + Object_Blur + Effect_Blur;   // 이펙트랑 위에 디퍼드를 바꿨다(이펙트 때문)
  
    if (Out.vColor.a == 0)
        discard;
    
    return Out;
}

/* ------------------- 5 -Effect Distortion Shader -------------------*/
PS_OUT PS_MAIN_EFFECT_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
       
    // 전처리에서 찍어준 디스토션 렌더타겟을 샘플링해서 얻어온다.
    vector Distortion = g_Effect_DistortionTarget.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = Distortion;
     
    // 왜곡된 텍스쿠드 좌표를 만든다.
    float2 vDistortedCoord = Distortion.xy + In.vTexcoord.xy;
     
    // 디퍼드 렌더타겟을 왜곡된 텍스쿠드 좌표로 샘플링한다.
    vector Deferred = g_Deferred_Target.Sample(LinearSampler, vDistortedCoord);
    
    if (Out.vColor.a == 0)
        discard;
    
    Out.vColor = Deferred;
    
    return Out;
}
/* ------------------- 6 - Vignette -------------------*/
PS_OUT PS_MAIN_VIGNETTE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector color = g_ProcessingTarget.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = vignette(color, In.vTexcoord);
    
    return Out;
}
/* ------------------- 7 - SSR -------------------*/
PS_OUT PS_MAIN_SSR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (0 == g_SSR_Desc.fStepCnt || EPSILON > g_SSR_Desc.fRayStep)
    {
        Out.vColor = g_ProcessingTarget.Sample(LinearSampler, In.vTexcoord);
        return Out;
    }
    float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
    
    float4 vNormal = g_NormalTarget.Sample(LinearSampler, In.vTexcoord);
    float4 vDepth = g_DepthTarget.Sample(LinearSampler, In.vTexcoord);
    float4 vProperties = g_ORMTarget.Sample(LinearSampler, In.vTexcoord);
    
    float fViewZ = vDepth.w * 1200.f;
    
    float4 vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vNormal.w;
    vWorldPos.w = 1.0f;
    
    vWorldPos *= fViewZ;
    //vWorldPos = mul(vWorldPos, g_ProjViewMatrixInv);
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    float4 vViewDir = normalize(vWorldPos - g_vCamPosition);
    vViewDir.w = 0.f;
    
    float4 vRayOrigin = vWorldPos;
    
    vNormal = float4(vNormal.xyz * 2.f - 1.f, 0.f);
    float4 vRayDir = normalize(reflect(vViewDir, vNormal));
    vRayDir.w = 0.f;
    
    float fStep = g_SSR_Desc.fRayStep;
    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float fPixelDepth = 0.f;
    int iStepDistance = 0;
    float2 vRayPixelPos = (float2) 0;
  
    [loop]
    for (iStepDistance = 1; iStepDistance < g_SSR_Desc.fStepCnt; ++iStepDistance)
    {
        float4 vDirStep = vRayDir * fStep * iStepDistance;
        vDirStep.w = 0.f;
        float4 vRayWorldPos = vRayOrigin + vDirStep;

        float4 vRayProjPos = mul(vRayWorldPos, matVP);
        vRayProjPos.x = vRayProjPos.x / vRayProjPos.w;
        vRayProjPos.y = vRayProjPos.y / vRayProjPos.w;
      
        vRayPixelPos = float2(vRayProjPos.x * 0.5f + 0.5f, vRayProjPos.y * -0.5f + 0.5f);
        
        clip(vRayPixelPos);
        clip(1.f - vRayPixelPos);
        
        float2 vPixelCoord = float2(0.f, 0.f);
        //vPixelCoord.x = g_NormalTarget.Sample(LinearSampler, vRayPixelPos).w;
        //vPixelCoord.y = g_NormalDepthTarget.Sample(LinearSampler, vRayPixelPos).w;
        
        fPixelDepth = g_NormalTarget.Sample(LinearSampler, vRayPixelPos).w;
        fPixelDepth *= g_DepthTarget.Sample(LinearSampler, vRayPixelPos).w * 1200.f;
        
        float fDiff = vRayProjPos.z - fPixelDepth;
        
        if (fDiff > 0.0f && fDiff < g_SSR_Desc.fRayStep)
            break;
    }
 
    clip(g_SSR_Desc.fStepCnt - 0.5f - iStepDistance);
    
    Out.vColor = g_ProcessingTarget.Sample(LinearSampler, vRayPixelPos) * (1.f - iStepDistance / g_SSR_Desc.fStepCnt) * (clamp(pow(vProperties.x, 2.2f) / (vProperties.y + EPSILON), 0.01f, 1.f));
    
    return Out;
}
// SSR
//    PS_OUT Out = (PS_OUT) 0;
    
//    // 반사되는 반직선을 계산하기위해 Depth 와 Normal의 방향을 사용한다. -> 반직선이 Geometry와 충돌할때까지 추적 
//    float4 normalMetallic = g_NormalTarget.Sample(LinearBorderSampler, In.vTexcoord);
//    float4 sceneColor = g_ProcessingTarget.SampleLevel(ClampSampler, In.vTexcoord, 0);

//    float metallic = normalMetallic.a;
//    if (metallic < 0.01f)
//    {
//        Out.vColor = sceneColor;
//        return Out;
//    }
    
//    float3 normal = normalMetallic.rgb;
//    normal = 2 * normal - 1.0;
//    normal = normalize(normal);

//    float4 DepthColor = g_DepthTarget.Sample(ClampSampler, In.vTexcoord);
//    float depth = DepthColor.r;
//    float3 viewSpacePosition = GetViewSpacePosition(In.vTexcoord, depth);
//    float3 reflectDirection = normalize(reflect(viewSpacePosition, normal));

//    float3 hitPosition = viewSpacePosition;
//    float4 coords = SSRRayMarch(reflectDirection, hitPosition);

//    float2 coordsEdgeFactor = float2(1, 1) - pow(saturate(abs(coords.xy - float2(0.5f, 0.5f)) * 2), 8);
//    float screenEdgeFactor = saturate(min(coordsEdgeFactor.x, coordsEdgeFactor.y));
//    float reflectionIntensity = saturate(screenEdgeFactor * saturate(reflectDirection.z) * (coords.w));

//    float3 reflectionColor = reflectionIntensity * g_ProcessingTarget.SampleLevel(ClampSampler, coords.xy, 0).rgb;
//    Out.vColor =  sceneColor + metallic * max(0, float4(reflectionColor, 1.0f));
   
//    return Out;
//} 
/* ------------------- 8 - Chroma-------------------*/
PS_OUT PS_MAIN_CHROMA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float  screenWidth, screenHeight;
    screenWidth = 1280.f;
    screenHeight = 720.f;
    //colorTex.GetDimensions(screenWidth, screenHeight);
    const float2 texelSize = 1.0.xx / float2(screenWidth, screenHeight);
  
    float2 center_offset = In.vTexcoord - float2(0.5, 0.5);

    float ca_amount = 0.018 * g_Chroma_Desc.fcaAmount;
	// ca_amount = 0.0;

	// Reduce the amount of CA in the center of the screen to preserve image sharpness.
    ca_amount *= saturate(length(center_offset) * 2);

    int num_colors = 7;
	//int num_colors = max(3, int(max(screenWidth, screenHeight) * 0.075 * sqrt(ca_amount)));
    float softness = 0.3;

    float3 color_sum = float3(0, 0, 0);
    float3 res_sum = float3(0, 0, 0);

    for (int i = 0; i < num_colors; ++i)
    {
        float t = float(i) / (num_colors - 1);

        const float thresh = softness * 2.0 / 3 + 1.0 / 3;
        float3 color =
			lerp(float3(0, 0, 1), float3(0, 0, 0), smoothstep(0, thresh, abs(t - 0.5 / 3)))
		    + lerp(float3(0, 1, 0), float3(0, 0, 0), smoothstep(0, thresh, abs(t - 1.5 / 3)))
		    + lerp(float3(1, 0, 0), float3(0, 0, 0), smoothstep(0, thresh, abs(t - 2.5 / 3)));

        color_sum += color;

        float offset = float(i - num_colors * 0.5) * ca_amount / num_colors;

        float2 sampleUv = float2(0.5, 0.5) + center_offset * (1 + offset);
        float3 Screen = g_ProcessingTarget.SampleLevel(LinearSampler, sampleUv, 0);
        res_sum += color * Screen;
    }

    float3 res = res_sum / color_sum;
    Out.vColor= float4(res, 1.0);
  
    return Out;
}

/* ------------------- 9 - Motion Blur -------------------*/
PS_OUT PS_MAIN_MOTIONBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float4 vDepth = g_DepthTarget.Sample(LinearSampler, In.vTexcoord);
    float4 vNormal = g_NormalTarget.Sample(LinearSampler, In.vTexcoord);
		   
    float fViewZ = vDepth.w * g_fCamFar;
		    
    float4 vPixelWorldPos, vPixelPos;

    vPixelWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vPixelWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vPixelWorldPos.z = vNormal.w;
    vPixelWorldPos.w = 1.0f;

    vPixelPos = vPixelWorldPos;
    vPixelWorldPos *= fViewZ;

    vPixelWorldPos = mul(vPixelWorldPos, g_ProjMatrixInv);
    vPixelWorldPos = mul(vPixelWorldPos, g_ViewMatrixInv);
    //vPixelWorldPos = mul(vPixelWorldPos, g_ProjViewMatrixInv);
    
    matrix matVP = mul(g_PreCamViewMatrix, g_CamProjMatrix);

    float4 vPrePixelPos = mul(vPixelWorldPos, matVP);
    vPrePixelPos /= vPrePixelPos.w;

    float2 vPixelVelocity = ((vPixelPos - vPrePixelPos) * 0.5f).xy;
    float2 texCoord = In.vTexcoord;

    float4 vColor = vector(0.f, 0.f, 0.f, 0.f);

    for (int i = -10; i < 10; ++i)
    {
        texCoord += vPixelVelocity * g_MotionBlur.fIntensity * i;
        float4 currentColor = g_ProcessingTarget.Sample(ClampSampler, texCoord);
        vColor += currentColor;
    }

    Out.vColor = vColor * 0.05f /*/ 20.f*/;
    
    return Out;
}
/* ------------------- 10 - Luma Sharpen -------------------*/
//  _____________________
//
//     LumaSharpen 1.4.1
//   _____________________
//
//  by Christian Cann Schuldt Jensen ~ CeeJay.dk
//  It blurs the original pixel with the surrounding pixels and then subtracts this blur to sharpen the image.
//  It does this in luma to avoid color artifacts and allows limiting the maximum sharpning to avoid or lessen halo artifacts.
//  This is similar to using Unsharp Mask in Photoshop.
//  Compiles with 3.0
PS_OUT PS_MAIN_LUMA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 c0 = g_ProcessingTarget.Sample(LinearSampler, In.vTexcoord);
    c0 = LumaSharpenPass(c0, In.vTexcoord);
    
    Out.vColor = c0;
    return Out;
}
/*=============================================================
 
                         Technique 
                                
==============================================================*/

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
        //SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetBlendState(BS_AlphaBlend_Effect, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECTMIX();
    }

    pass Effect_Distortion // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT_DISTORTION();
    }

    pass VIGNETTE // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_VIGNETTE();
    }

    pass SSR // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSR();
    }

    pass Chroma // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CHROMA();
    }

    pass MotionBlur //9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MOTIONBLUR();
    }

    pass LumaSharpen // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LUMA();
    }
}