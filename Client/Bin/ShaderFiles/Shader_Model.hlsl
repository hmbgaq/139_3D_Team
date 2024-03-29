#include "Shader_Defines.hlsli"

/*=============================================================
 
                             Value
                                
==============================================================*/
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4          g_vCamPosition;
float           g_fCamFar;
float           g_fLightFar;
float           g_fTimeDelta;
float4          g_vDiffuseColor = { 1.f, 1.f, 1.f, 1.f };


/* =========== Texture =========== */
Texture2D		g_DiffuseTexture;
Texture2D       g_NormalTexture;
Texture2D       g_SpecularTexture;
Texture2D       g_EmissiveTexture;
Texture2D       g_OpacityTexture;
Texture2D       g_AmbientOcclusionTexture;
Texture2D       g_RoughnessTexture;
Texture2D       g_MetalicTexture;

Texture2D       g_MaskTexture;
Texture2D       g_NoiseTexture;
Texture2D       g_ColorDiffuse;

Texture2D       g_RADTexture;

/* =========== Shader Value =========== */
float   g_fDissolveWeight;                          /* Dissolve */
                                                    
float4  g_vLineColor;                               /* OutLine */
float   g_LineThick;                                /* OutLine */
                                                    
float3  g_vBloomPower = { 0.f, 0.f, 0.f };          /* Bloom */
float4  g_vRimColor = { 0.f, 0.f, 0.f, 0.f };       /* RimLight */
float   g_fRimPower = 5.f;

float   g_fReflectionScale = 0.05f;                 /* Icicle */ 

/*=============================================================
 
                             Function 
                                
==============================================================*/
float2 RotateTexture(float2 texCoord, float angle)
{
    float2 rotatedTexCoord;
    rotatedTexCoord.x = texCoord.x * cos(angle) - texCoord.y * sin(angle);
    rotatedTexCoord.y = texCoord.x * sin(angle) + texCoord.y * cos(angle);
    
    return rotatedTexCoord;
    
}

float4 Calculation_RimColor(float4 In_Normal, float4 In_Pos)
{
    float fRimPower = 1.f - saturate(dot(In_Normal, normalize((-1.f * (In_Pos - g_vCamPosition)))));
    fRimPower = pow(fRimPower, g_fRimPower); // 여기서 강도를 조정한다. 
    
    float4 vRimColor = g_vRimColor * fRimPower;
    
    return vRimColor;
}

float4 Calculation_Brightness(float4 Out_Diffuse)
{
    float4 vBrightnessColor = float4(0.f, 0.f, 0.f, 0.f);

    float fPixelBrightness = dot(Out_Diffuse.rgb, g_vBloomPower.rgb);
    
    if (fPixelBrightness > 0.99f)
        vBrightnessColor = float4(Out_Diffuse.rgb, 1.0f);

    return vBrightnessColor;
}

/*=============================================================
 
                      Constant Buffer
                                
==============================================================*/
cbuffer VS_CONSTANT_BUFFER
{
    matrix  mWorldViewProj;
    float4  vSomeVectorThatMayBeNeededByASpecificShader;
    float   fSomeFloatThatMayBeNeededByASpecificShader;
    float   fTime;
    float   fSomeFloatThatMayBeNeededByASpecificShader2;
    float   fSomeFloatThatMayBeNeededByASpecificShader3;
};

/*=============================================================
 
                             Struct
                                
==============================================================*/
struct VS_IN
{
	float3		vPosition       : POSITION;
	float3		vNormal         : NORMAL;
	float2		vTexcoord       : TEXCOORD0;
	float3		vTangent        : TANGENT;
};

struct VS_OUT
{
	float4		vPosition       : SV_POSITION;
	float4		vNormal         : NORMAL;
	float2		vTexcoord       : TEXCOORD0;	
	float4		vWorldPos       : TEXCOORD1;
	float4		vProjPos        : TEXCOORD2;
	float4		vTangent        : TANGENT;
	float4		vBinormal       : BINORMAL;
};

struct VS_OUT_ICICLE
{
    float4      vPosition       : SV_POSITION;
    float4      vNormal         : NORMAL;
    float2      vTexcoord       : TEXCOORD0;
    float4      vWorldPos       : TEXCOORD1;
    float4      vProjPos        : TEXCOORD2;
    float4      vTangent        : TANGENT;
    float4      vBinormal       : BINORMAL;
    float4      vRefractionPos  : TEXCOORD3;
};

struct PS_IN
{
    float4      vPosition       : SV_POSITION;
    float4      vNormal         : NORMAL;
    float2      vTexcoord       : TEXCOORD0;
    float4      vWorldPos       : TEXCOORD1;
    float4      vProjPos        : TEXCOORD2;
    float4      vTangent        : TANGENT;
    float4      vBinormal       : BINORMAL;
};

struct PS_IN_ICICLE
{
    float4      vPosition       : SV_POSITION;
    float4      vNormal         : NORMAL;
    float2      vTexcoord       : TEXCOORD0;
    float4      vWorldPos       : TEXCOORD1;
    float4      vProjPos        : TEXCOORD2;
    float4      vTangent        : TANGENT;
    float4      vBinormal       : BINORMAL;
    float4      vRefractionPos  : TEXCOORD3;
};

struct PS_OUT
{
    float4      vDiffuse        : SV_TARGET0;
    float4      vNormal         : SV_TARGET1;
    float4      vDepth          : SV_TARGET2;
    float4      vORM            : SV_TARGET3;
    float4      vRimBloom       : SV_TARGET4; /* Rim + Bloom */
    float4      vEmissive       : SV_Target5;
};

struct PS_ICEGROUP
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal  : SV_TARGET1;
};

struct PS_OUT_SHADOW
{
    vector vLightDepth : SV_TARGET0;
};

/*=============================================================
 
                        Vertex Shader
                                
==============================================================*/

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

VS_OUT VS_MAIN_OUTLINE(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float4 OutPos = vector(In.vPosition.xyz + In.vNormal.xyz * g_LineThick, 1);
    Out.vPosition = mul(OutPos, matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

VS_OUT_ICICLE VS_MAIN_ICICLE(VS_IN In)
{
    VS_OUT_ICICLE Out = (VS_OUT_ICICLE) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

    matrix ViewProjectWorld;
    ViewProjectWorld = mul(g_ViewMatrix, g_ProjMatrix);
    ViewProjectWorld = mul(g_WorldMatrix, ViewProjectWorld);
    Out.vRefractionPos = mul(float4(In.vPosition, 1.f), ViewProjectWorld);
    
    return Out;
}

/*=============================================================
 
                        Pixel Shader
                                
==============================================================*/
/* ------------------ Base Pixel Shader (0) ------------------*/

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.0f)
        discard;
    
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal = mul(vPixelNormal, WorldMatrix);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    //vector vO = g_AmbientOcclusionTexture.Sample(LinearSampler, In.vTexcoord); //AO 는 HBAO+ 가 할거라 필요없음. 
    //vector vR = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    //vector vM = g_MetalicTexture.Sample(LinearSampler, In.vTexcoord);
    
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
PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    Out.vLightDepth = In.vProjPos.w / 500.f;
	
    return Out;
}

/* ------------------- Shadow Pixel Shader(3) -------------------*/
PS_OUT PS_MAIN_WHITE_BLINK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vColor;
    
    /* Normal Setting */ 
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    vColor = lerp(vMtrlDiffuse, mul(vMtrlDiffuse, float4(0.7f, 0.7f, 0.7f, 0.7f)), g_fTimeDelta);

    Out.vDiffuse = vColor;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    return Out;
}

/* ------------------- OutLine Pixel Shader(4) -------------------*/
PS_OUT PS_MAIN_OUTLINE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vColor = { 1.f, 1.f, 1.f, 1.f };
    vColor.a = g_fTimeDelta;
    
    /* Normal Setting */ 
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vDiffuse = vColor;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    return Out;
}

/* ------------------- (5) IntroBoss BloodPool -------------------*/
// NoneCull모드일뿐 동일함 

/* ------------------- (6) IntroBoss BloodPool -------------------*/
PS_OUT PS_BloodPool(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fTime = g_fTimeDelta;
    float2 newUV = RotateTexture(In.vTexcoord, fTime);
   
    vector vMtrlDiffuse = g_ColorDiffuse.Sample(LinearSampler, newUV);
    vector vMtrlMask = g_MaskTexture.Sample(LinearSampler, newUV);
    vector vMtrlNoise = g_NoiseTexture.Sample(LinearSampler, newUV);
    
    if (vMtrlMask.r <= 0.6f)
        Out.vDiffuse = vector(0.275f, 1.f, 0.f, 1.f);
    else
        Out.vDiffuse = vector(1.f, 0.039f, 0.f, 1.f);
    
    //if (vMtrlDiffuse.a < 0.0f)
    //    discard;
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal = mul(vPixelNormal, WorldMatrix);
    
    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}

/* ------------------- (7) Normal Mapping -------------------*/
PS_OUT PS_MAIN_NORMAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.0f)
        discard;
    
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal = mul(vPixelNormal, WorldMatrix);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}

/* ------------------- (8) IntroBoss BloodPool -------------------*/
PS_OUT PS_MAIN_NORMALCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_ColorDiffuse.Sample(LinearSampler, In.vTexcoord);

    //vMtrlDiffuse.rgb = g_ColorDiffuse.rgb;
    
    if (vMtrlDiffuse.a <= 0.0f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz;
    vPixelNormal = vPixelNormal * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vPixelNormal = mul(vPixelNormal, WorldMatrix);
    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    /* ---------------- New ---------------- */
    //float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    //Out.vDiffuse += vRimColor;
    //Out.vRimBloom = Calculation_Brightness(Out.vDiffuse); // + vRimColor;
 
    return Out;
}

/* ------------------- (9) ALPHACOLOR -------------------*/
PS_OUT PS_MAIN_ALPHACOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMtrlOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
    
    //float4 vTest = { 0.1f, 0.1f, 0.1f, 1.f };
    //if (vMtrlDiffuse.rgb > vTest.rgb)
    if (vMtrlOpacity.r <= 0)
    {
        Out.vDiffuse = g_ColorDiffuse.Sample(LinearSampler, In.vTexcoord);
    }
    else
    {
        Out.vDiffuse = g_vDiffuseColor;
        Out.vRimBloom = Calculation_Brightness(Out.vDiffuse); // + vRimColor;
        
    }
    
    //if (vMtrlOpacity.a == 0)
    //{
    //    Out.vDiffuse = g_ColorDiffuse.Sample(LinearSampler, In.vTexcoord);
    //}
    //else
    //{
    //    Out.vDiffuse = g_vDiffuseColor;
    //    Out.vRimBloom = Calculation_Brightness(Out.vDiffuse); // + vRimColor;
    //}
    
    //vMtrlDiffuse.rgb = g_vDiffuseColor.rgb;
    
    //if (vMtrlDiffuse.a < 0.0f)
    //    discard;
    
    /* 0 ~ 1 */
    float3 vPixelNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz;

	/* -1 ~ 1 */
    vPixelNormal = vPixelNormal * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    vPixelNormal = mul(vPixelNormal, WorldMatrix);
    
    //Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vPixelNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vORM = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    /* ---------------- New ---------------- */
    //float4 vRimColor = Calculation_RimColor(In.vNormal, In.vWorldPos);
    //Out.vDiffuse += vRimColor;
    //Out.vRimBloom = Calculation_Brightness(Out.vDiffuse); // + vRimColor;
 
    return Out;
}

/* ------------------- (10) Icicle -------------------*/
PS_OUT PS_MAIN_ICICLE(PS_IN_ICICLE In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    /* 좌표를 (-1, 1) -> (0, 1) 로 변환 */ 
    float2 RefractTexCoord;
    RefractTexCoord.x = In.vRefractionPos.x / In.vRefractionPos.w / 2.0f + 0.5f;
    RefractTexCoord.y = -In.vRefractionPos.y / In.vRefractionPos.w / 2.0f + 0.5f;
    
    /* 노말맵은 (0, 1) 의 좌표를 (-1, 1)로 변환 */
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = (vNormalDesc.xyz * 2.f) -1.f;
    
    /* 굴절크기변수를 노말값에 곱하기 + 흩뜨려놓기 */ 
    RefractTexCoord = RefractTexCoord + (vNormal.xy * g_fReflectionScale);
    
    float4 RefractionColor = g_NoiseTexture.Sample(LinearSampler, RefractTexCoord);    
    vector vMtrlDiffuse = g_ColorDiffuse.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = lerp(RefractionColor, vMtrlDiffuse, 0.5f);
    
    /* 현재 픽셀까지의 거리를 계산 */
    float distanceToCamera = distance(In.vRefractionPos, g_vCamPosition);
    
    /* 거리에 따라 투명도를 조절 (예시로 설정한 값) */
    float maxDistance = 30.0f; // 최대 거리
    float minTransparency = 0.2f; // 최소 투명도
    float maxTransparency = 1.0f; // 최대 투명도
    float transparency = lerp(maxTransparency, minTransparency, saturate(distanceToCamera / maxDistance));
    
    /* 최종 색상을 계산할 때 알파 값을 조절 */
    Out.vDiffuse = lerp(RefractionColor, vMtrlDiffuse, 0.5f);
    Out.vDiffuse.a *= transparency;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    Out.vNormal = (vector(vNormal * 0.5f + 0.5f, 0.f)) * 0.2;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}

PS_ICEGROUP PS_MAIN_ICEGROUP(PS_IN_ICICLE In)
{
    /* 얼음, 물 전용으로 아에 RenderGroup을 빼서 하는것 전용 */
    
    PS_ICEGROUP Out = (PS_ICEGROUP) 0;
 
    /* 좌표를 (-1, 1) -> (0, 1) 로 변환 */ 
    float2 RefractTexCoord;
    RefractTexCoord.x = In.vRefractionPos.x / In.vRefractionPos.w / 2.0f + 0.5f;
    RefractTexCoord.y = -In.vRefractionPos.y / In.vRefractionPos.w / 2.0f + 0.5f;
    
    /* 노말맵은 (0, 1) 의 좌표를 (-1, 1)로 변환 */
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = (vNormalDesc.xyz * 2.f) - 1.f;
    
    /* 굴절크기변수를 노말값에 곱하기 + 흩뜨려놓기 */ 
    RefractTexCoord = RefractTexCoord + (vNormal.xy * g_fReflectionScale);
    
    float4 RefractionColor = g_NoiseTexture.Sample(LinearSampler, RefractTexCoord);
    vector vMtrlDiffuse = g_ColorDiffuse.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = lerp(RefractionColor, vMtrlDiffuse, 0.5f);
    
    /* 현재 픽셀까지의 거리를 계산 */
    float distanceToCamera = distance(In.vRefractionPos, g_vCamPosition);
    
    /* 거리에 따라 투명도를 조절 (예시로 설정한 값) */
    float maxDistance = 30.0f; // 최대 거리
    float minTransparency = 0.2f; // 최소 투명도
    float maxTransparency = 1.0f; // 최대 투명도
    float transparency = lerp(maxTransparency, minTransparency, saturate(distanceToCamera / maxDistance));
    
    /* 최종 색상을 계산할 때 알파 값을 조절 */
    Out.vDiffuse = lerp(RefractionColor, vMtrlDiffuse, 0.5f);
    Out.vDiffuse.a *= transparency;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    Out.vNormal = (vector(vNormal * 0.5f + 0.5f, 0.f)) * 0.2;
    //Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.0f, 0.0f);
    //Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}
/*=============================================================
 
                          Technique
                                
==============================================================*/

technique11 DefaultTechnique
{	
	pass Model // 0번 패스
	{		
		SetRasterizerState(RS_Cull_None);
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

    pass White_Blink // 3 - Interact Chain전용 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WHITE_BLINK();
    }

    pass OutLine // 4
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_OUTLINE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
    }

    pass Model_NoneCull_NoneDSS //5번 패스
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Model_BloodPool // 6번 패스 보스맵 피웅덩이 전용
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BloodPool();
    }

    pass Model_NormalMapping // 7번 패스
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
    }

    pass Model_InputColor   // 8번 패스 컬러 디퓨즈 컬러 직접 던지기용
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMALCOLOR();
    }
       
    pass Model_AlphaTexture // 9번 패스
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHACOLOR();
    }
    pass Icicle // 10
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_ICICLE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ICICLE();
    }

    pass RenderICE_GROUP
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_ICICLE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ICEGROUP();
    }
}
