#ifndef SHADER_DEFINES_HLSLI
#include "Shader_Defines.hlsli"
#define SHADER_DEFINES_HLSLI
#endif

/* ------------ 공 지 사 항 ----------------- */
// 보통 ORM텍스쳐를 Occulusuin, Roughness, Metalic 이라고 하는데 
// 우리는 ORM텍스쳐가 있기때문에 R (Roughness), G (Metallic), B (Ambient Occlusion) 에서 가져와야한다. 
/* ----------------------------------------- */

/* ----------------- Variable ----------------- */
TextureCube g_IrradianceTexture;
TextureCube g_PreFilteredTexture;
Texture2D   g_BRDFTexture;
float4      g_LightDiffuse = { 1.f, 1.f, 1.f, 1.f };
/* -------------------------------------------- */

// Burley B. "Physically Based Shading at Disney"
// SIGGRAPH 2012 Course: Practical Physically Based Shading in Film and Game Production, 2012.
float3 Disney_Diffuse(in float roughnessPercent, in float3 diffuseColor, in float NdotL, in float NdotV, in float LdotH)
{
    float energyBias = lerp(0.0f, 0.5f, roughnessPercent);
    float energyFactor = lerp(1.0f, 1.0f / 1.51f, roughnessPercent);
   
    float fd90 = energyBias + 2.0f * roughnessPercent * LdotH * LdotH;
    
    float lightScatter = 1.0f + (fd90 - 1.0f) * pow(1.0f - NdotL, 5.0f);
    float viewScatter = 1.0f + (fd90 - 1.0f) * pow(1.0f - NdotV, 5.0f);
    
    return diffuseColor * lightScatter * viewScatter * energyFactor;
    
    //float albedoLuminosity = 0.3 * diffuseColor.r
    //            + 0.6 * diffuseColor.g
    //            + 0.1 * diffuseColor.b;
    //        // normalize luminosity to isolate hue and saturation
    //float3 albedoTint = albedoLuminosity > 0 ?
    //                            diffuseColor / albedoLuminosity :
    //                            float3(1, 1, 1);
    //
    //float fresnelL = pow(clamp(1 - NdotL, 0, 1), 5);
    //float fresnelV = pow(clamp(1 - NdotV, 0, 1), 5);
    //
    //float fresnelDiffuse = 0.5 + 2 * pow(LdotH, 2.f) * roughnessPercent;
    //
    //float diffuse = albedoTint
    //                        * lerp(1.0, fresnelDiffuse, fresnelL)
    //                        * lerp(1.0, fresnelDiffuse, fresnelV);
    //
    //float fresnelSubsurface90 = pow(LdotH, 2.f) * roughnessPercent;
    //
    //float fresnelSubsurface = lerp(1.0, fresnelSubsurface90, fresnelL)
    //                                * lerp(1.0, fresnelSubsurface90, fresnelV);
    //
    //float ss = 1.25 * (fresnelSubsurface * (1 / (NdotL + NdotV) - 0.5) + 0.5);
    //
    //return saturate(lerp(diffuse, ss, 0.5f /*_Subsurface*/) * (1 / PI) * diffuseColor);
}


float D_GGX(float roughness, float NoH, const float3 NxH)
{
    float a = NoH * roughness;
    float k = roughness / (dot(NxH, NxH) + a * a);
    float d = k * k * (1.0 / PI);
    return min(d, 65504.0);
}

// GGX Specular D (normal distribution)
// https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
float D_GGX(in float roughness2, in float NdotH)
{
    float alpha = roughness2 * roughness2;
    const float NdotH2 = NdotH * NdotH; // NdotH2 = NdotH^2
   
    //alpha = pow(alpha, 2.f); // 언리얼은 4제곱

    const float lower = (NdotH2 * (alpha - 1.0f)) + 1.0f;
    //const float lower = NdotH2 * alpha + (1.0f - NdotH2);
    return alpha / (PI * lower * lower);   
}

// Shlick's approximation of Fresnel By Unity Engine
float3 F_Shlick(in float3 specularColor, in float HdotV)
{
    float FC = pow(1.0f - HdotV, 5.0f);
    return specularColor + (float3(1.0f, 1.0f, 1.0f) - specularColor) * FC;
}

// Schlick-Smith specular G (visibility) By Unity Version
float G_Smith(float roughness2, float NdotV, float NdotL)
{
    float SmithV = NdotL * sqrt(NdotV * (NdotV - NdotV * roughness2) + roughness2);
    float SmithL = NdotV * sqrt(NdotL * (NdotL - NdotL * roughness2) + roughness2);
    
    return 0.5f / max(SmithV + SmithL, 1e-5f);
}

float GGX_Geometry(float cosThetaN, float roughness4)
{
    float cosTheta_sqr = saturate(cosThetaN * cosThetaN);
    float tan2 = (1.0f - cosTheta_sqr) / cosTheta_sqr;
    
    return 2.0f / (1.0f + sqrt(1.0f + roughness4 * tan2));
}

float G_GGX(in float roughness2, in float NdotV, in float NdotL)
{
    float alpha = roughness2 * roughness2;
    
    return GGX_Geometry(NdotV, alpha) * GGX_Geometry(NdotL, alpha);
}

float G_SmithShlick(in float roughness2, in float NdotV, in float NdotL)
{
    float r = sqrt(roughness2) + 1.0f;
    float k = (r * r) / 8.0f;
    
    float SmithV = NdotV / (NdotV * (1.0 - k) + k);
    float SmithL = NdotL / (NdotL * (1.0 - k) + k);
    
    return SmithV * SmithL;
}

/*=============================================================
 
                             다른거 
                                
==============================================================*/

// 3. Fresnel Equation - 프레넬방정식
// 우리가 보는 각도에 따른 반사율과 굴절율을 계산한다. 
// -> 매우 복잡한 방정식을 가지고 있으나 슐릭(Schlick)의 근사식으로 간단하게 구할 수 있다.
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    // 가까운물은 바닥까지 잘 보이고 먼 물은 거울처럼 반사되어 보이는것
    // 빛은 다른 매질과 충돌할 떄 일정 비율은 굴절되고 일정 비율은 반사가 되는데 
    // 입사각에 따라 반사, 굴절의 정도가 달라지는 현상을 표현한것이 프레넬 방정식이다. 
    // 직접 계산하기에는 매우 복잡해서 PBR에서는 근사화한 버전을 사용한다 = 슐릭의 근사식
   
    // costheta는 (half way 벡터 * view벡터)로 쓰이는값이다. 
    return F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f);
    
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;
	
    return num / denom;
}

// 2. Geometry Function 
// Roughness를 인자로 받아서 표면이 서로 겹쳐서 빛을 차단하는 정도의 근사치를 통계적으로 구한다. 
// k는 direct lighting이냐 IBL Lighting이냐에 따라서 달라질 수 있다. 
float GeometrySchlickGGX(float NdotV, float roughness) // k is a remapping of roughness based on direct lighting or IBL lighting
{
    // k : roughness값의 remapping (direct light, IBL 등 경우에 따라 다름)
    // 그리고 아웃풋은 얼마나 미세면그림자가 져있는지가 나온다. 
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0; // direct
    
    //float k = (roughness * roughness) / 2.0f; // IBL 

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
// 
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

// 1. Normal Distribution Function (NDF)
// half way vector h 와 일치하는 microfacets의 표면 영역의 근사치를 구한다. 
// roughness가 0에 가까우면 매우 작은 반경에 집중되어 halfway vector와 일치하는 microfacets의 근사치를 구한다. -> 매우 밝은 색상을 띄게된다. 
// 1에 가까우면 더 많이 halfway vector와 일치하겠지만 자긍ㄴ 한 반경에 집중되지않아서 어느 특정부부남ㄴ 엄청 밝은색을 띄는게 아니라 전체적으로 회색 색상을 띄게된다. 
float NormalDistributionGGXTR(float NdotH, float roughness)
{
    // roughness값이 클수록 highlight되는 영역이 넓어지면서 희미해진다. 
    float roughness2 = roughness * roughness;
    float NdotH2 = NdotH * NdotH;

    float nom = roughness2;
    float denom = (NdotH2 * (roughness2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}

float3 FresnelSchlickFrostbite(float3 F0, float F90, float u)
{
    return F0 + (F90 - F0) * pow(1 - u, 5);
}

float DisneyFrostbiteDiff(in float roughness, in float3 diffuseColor, in float NdotL, in float NdotV, in float LdotH)
{
    float energyBias = lerp(0.f, 0.5f, roughness);
    float energyFactor = lerp(1.0f, 1.0f / 1.51f, roughness);
    float Fd90 = energyBias + 2.0f * pow(LdotH, 2.f) * roughness;
    float3 F0 = float3(1.f, 1.f, 1.f);
    float lightScatter = FresnelSchlickFrostbite(F0, Fd90, NdotL).r;
    float viewScatter = FresnelSchlickFrostbite(F0, Fd90, NdotV).r;
    return (lightScatter * viewScatter * energyFactor * diffuseColor) / PI;
}

// GGX Specular D
float Specular_D_GGX(in float roughness2, in float NdotH)
{
    const float lower = (NdotH * (NdotH * roughness2 - NdotH)) + 1.0f;
    return roughness2 / (PI * lower * lower);
}


// Shlick's approximation of Fresnel By Unity Engine
float3 Specular_F_Fresnel_Shlick_Unity(in float3 specularColor, in float LdotH)
{
    float FC = pow(1.0f - LdotH, 5.0f);
    return specularColor + (1.0f - specularColor) * FC;
}

float3 Specular_BRDF(in float roughness2, in float3 specularColor, in float NdotH, in float NdotV, in float NdotL, in float LdotH)
{
    // Specular D
    float specular_D = Specular_D_GGX(roughness2, NdotH);
    
    // Specular G
    float specular_G = G_Smith(roughness2, NdotV, NdotL);
    
    // Specular F
    float3 specular_F = Specular_F_Fresnel_Shlick_Unity(specularColor, LdotH);

    return (specular_D * specular_G) * specular_F;
}

/*=============================================================
 
                             BRDF 
                                
==============================================================*/

// Result = BRDF / 양방향 반사분포함수 
// 인자 두개를 받고 그 관계에 따라 반사율 (반사광 / 입사광)을 출력하는 함수. 
// Mtrl의 반사와 굴절 속성의 근사치를 micofacet이론을 바타응로 구한다. 
// 대부분 실시간 렌더링파이프라인에서는 Cook-Torrance BRDF를 사용한다. 

float3 BRDF(in float roughness2, in float fMetallic, in float3 vDiffuseColor, in float3 vSpecularColor, in float3 N, in float3 V, in float3 L, in float3 H)
{
    const float NdotL = max(dot(N, L), EPSILON);
    const float NdotV = max(dot(N, V), EPSILON);
    const float NdotH = max(dot(N, H), EPSILON);
    const float HdotV = max(dot(H, V), EPSILON);
    
    // Distribution & Geometry & Fresnel
    float D = D_GGX(roughness2, NdotH);
    float G = G_GGX(roughness2, NdotV, NdotL);
    float3 F = F_Shlick(vSpecularColor, HdotV);
    
    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - fMetallic;
    
    // Diffuse & Specular factors
    float denom = max(4.0f * NdotV * NdotL, 0.001f); // 0.001f just in case product is 0
    float3 specular_factor = saturate((D * F * G) / denom); // Cook-Torrance Specula
    float3 diffuse_factor = kD * vDiffuseColor / PI; // Lambertian diffuse - diffuse모델의 경우 거의 모든 BRDF 공식에서 별 차이가 없다 
    // -> EPIC GAMES에서 발표한 결과에 따르면 Lambertian diffuse가 대부분의 리얼타임 렌더링에서 충분하다고 함.
    // -> Diffuse BRDF가 에너지 보존법칙을 고려하게되면 위의 공식에 의해 Diffuse맵을 Albedo(RBG 채널당 표면 반사율)의 개념으로 사용하게된다. 
    // 이경우 알베도는 절대값인 색상값이 아니라 비율이기때문에 "데이터"의 의미로 들어가기떄문에 다른 파라미터를 통해서 밝기를 조절해야된다. 
    // 조절을 안할경우 /PI 이기때문에 Diffuse가 1/3정도 줄어들게된다. 
    
    return (diffuse_factor + specular_factor) * NdotL;
    
}

//float3 PBR_DirectionalLight(in float3 V, in float3 N, in DirectionalLight light,
//    in float3 albedo, in float ambientOcclusion, in float roughness, in float metallic, in float shadow)
//{
//    // Output color
//    float3 acc_color = float3(0.0f, 0.0f, 0.0f);
    
//    // Burley roughness bias
//    const float roughness2 = max(roughness * roughness, 0.01f);
    
//    // Blend base colors
//    const float3 c_diff = lerp(albedo, float3(0, 0, 0), metallic) * ambientOcclusion;
//    const float3 c_spec = lerp(F_ZERO, albedo, metallic) * ambientOcclusion;

//    // Calculate Directional Light
//    const float3 L = normalize(-light.Direction);
//    const float3 H = normalize(V + L);
    
//    // products
//    const float NdotL = max(dot(N, L), EPSILON);
//    const float NdotV = abs(dot(N, V)) + EPSILON;
//    const float NdotH = max(dot(N, H), EPSILON);
//    const float HdotV = max(dot(H, V), EPSILON);

//    // BRDF
//    float3 brdf_factor = BRDF(roughness2, metallic, c_diff, c_spec, NdotH, NdotV, NdotL, HdotV);
    
//    // Directional light
//    acc_color += light.Diffuse.rgb * light.Power * shadow * brdf_factor;

//    return acc_color;
//}


float3 New_BRDF(in float fRoughness, in float fMetallic, in float3 vDiffuseColor, in float3 F0, in float3 N, in float3 V, in float3 L, in float3 H, in float fAO)
{   
    const float NdotL = max(dot(N, L), EPSILON); // Normal vector [dot] Light vector  -> 결과는 costheta이다. N과 L은 정규화된 벡터이므로 그 길이가 1이기 때문임. 
    const float NdotV = max(dot(N, V), EPSILON); // 
    const float NdotH = max(dot(N, H), EPSILON); // Normal vector [dot] halfway vector  
    const float HdotV = max(dot(H, V), EPSILON); // halfway vector [dot] view vector
    
    float3 R = reflect(-V, N);
    
    // EpicGame's Unreal Engine4 가 채용중인 PBR : Trowbridge-Reitz GGX(D), Fresnel-Schlick(F), 그리고 Smith's Schlick-GGX(G)
    // pbr에서 사용되는 D, F, G 는 여러가지 버전이 있을 수 있다. 계산속도를 중시한다던가 물리적으로 사실적인가 등을 고려하기 떄문이다. 
    float D = NormalDistributionGGXTR(NdotH, fRoughness); // roughness값에 의한 반사분포
    float3 F = fresnelSchlick(HdotV, F0); // 다른 각도로 봤을 떄 달라지는 반사정도 - 금석성과 관련이 크다. 
    float G = GeometrySmith(NdotV, NdotL, fRoughness); // 미세면 그림자
    
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= (1.0f - fMetallic);
    
    // Diffuse & Specular factors
    //float denom = max(4.0f * NdotV * NdotL, 0.001f); // 0.001f just in case product is 0
    float3 numerator = D * G * F;
    float denom = 4.0f * NdotV * NdotL;
    float3 specular_factor = numerator / max(denom, 0.001);
    
    float3 vRadiance = g_LightDiffuse.xyz * EPSILON /*attenuation*/;
    float3 Lo = (kD * vDiffuseColor / PI + specular_factor) * vRadiance * NdotL;
    
    float3 F1 = FresnelSchlickRoughness(NdotV, F0, fRoughness);
    
    kS = F1;
    kD = 1.0 - kS;
    kD *= 1.0 - fMetallic;
    
    float3 vIrradiance = g_IrradianceTexture.Sample(ClampSampler, N).rgb;
    float3 vDiffuse = vIrradiance * vDiffuseColor.xyz;
    
    const float MAX_REFLECTION_LOD = 9.0f;
    
    float3 prefilteredColor = g_PreFilteredTexture.SampleLevel(ClampSampler, R, fRoughness * MAX_REFLECTION_LOD).rgb;
    float2 envBRDF = g_BRDFTexture.Sample(ClampSampler, float2(NdotV, fRoughness)).rg;
    float3 specular = prefilteredColor * (F1 * envBRDF.x + envBRDF.y);
    
    float3 vAmbient = (kD * vDiffuse + specular) * fAO;
    
    float3 vColor = vAmbient + Lo;
    
    return vColor;
}

float3 MY_BRDF(in float fRoughness, in float fMetallic, in float3 vDiffuseColor, in float3 F0, in float3 N, in float3 V, in float3 L, in float3 H, in float fAO)
{
    const float NdotL = saturate(dot(N, L));
    const float NdotV = saturate(dot(N, V));
    const float NdotH = saturate(dot(N, H));
    const float HdotV = saturate(dot(H, V));
    
    // EpicGame's Unreal Engine4 가 채용중인 PBR : Trowbridge-Reitz GGX(D), Fresnel-Schlick(F), 그리고 Smith's Schlick-GGX(G)
    // pbr에서 사용되는 D, F, G 는 여러가지 버전이 있을 수 있다. 계산속도를 중시한다던가 물리적으로 사실적인가 등을 고려하기 떄문이다. 
    float D = NormalDistributionGGXTR(NdotH, fRoughness); // roughness값에 의한 반사분포
    float G = GeometrySmith(NdotV, NdotL, fRoughness); // 미세면 그림자
    float3 F = fresnelSchlick(HdotV, F0); // 다른 각도로 봤을 떄 달라지는 반사정도 - 금석성과 관련이 크다. 
    
    float3 vColor = F;
    
    return vColor;
}