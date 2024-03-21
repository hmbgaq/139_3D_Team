#ifndef _SHADER_PBR_HLSL_
#define _SHADER_PBR_HLSL_

#include "Shader_Defines.hlsli"

static const float PI = 3.14159265359f;
static const float EPSILON = 0.000001f;

/* ----------------- Variable ----------------- */
TextureCube     g_IrradianceTexture;
TextureCube     g_PreFilteredTexture;
Texture2D       g_BRDFTexture;

float4          g_vLightDiffuse = { 1.f, 1.f, 1.f, 1.f };

/* -------------------------------------------- */

float3 fresnelSchlick(float cosTheta, float3 F0)
{
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

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
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
   
    alpha = pow(alpha, 2.f); // 언리얼은 4제곱
    const float NdotH2 = NdotH * NdotH; // NdotH2 = NdotH^2

    //const float lower = (NdotH2 * (alpha - 1.0f)) + 1.0f;
    const float lower = NdotH2 * alpha + (1.0f - NdotH2);
    return alpha / (PI * lower * lower);
}

float NormalDistributionGGXTR(float NdotH, float roughness)
{
    float roughness2 = roughness * roughness;
    float roughness4 = roughness2 * roughness2;
    float NdotH2 = NdotH * NdotH;

    float nom = roughness4;
    float denom = (NdotH2 * (roughness4 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}

// Shlick's approximation of Fresnel By Unity Engine
float3 F_Shlick(in float3 specularColor, in float HdotV)
{
    float FC = pow(1.0f - HdotV, 5.0f);
    return specularColor + (float3(1.0f, 1.0f, 1.0f) - specularColor) * FC;
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

// Schlick-Smith specular G (visibility) By Unity Version
float Specular_G_Smith_Unity(float roughness2, float NdotV, float NdotL)
{
    float SmithV = NdotL * sqrt(NdotV * (NdotV - NdotV * roughness2) + roughness2);
    float SmithL = NdotV * sqrt(NdotL * (NdotL - NdotL * roughness2) + roughness2);
    
    return 0.5f / max(SmithV + SmithL, 1e-5f);
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
    float specular_G = Specular_G_Smith_Unity(roughness2, NdotV, NdotL);
    
    // Specular F
    float3 specular_F = Specular_F_Fresnel_Shlick_Unity(specularColor, LdotH);

    return (specular_D * specular_G) * specular_F;
}

/* -------------------------------------------- */
float3 BRDF(in float fRoughness, in float fMetallic, in float3 vDiffuseColor, in float3 F0, in float3 N, in float3 V, in float3 L, in float3 H, in float fAO)
{
    const float NdotL = max(dot(N, L), EPSILON);
    const float NdotV = max(dot(N, V), EPSILON);
    const float NdotH = max(dot(N, H), EPSILON);
    const float HdotV = max(dot(H, V), EPSILON);
    
    // Distribution & Geometry & Fresnel
    //float NDF = DistributionGGX(N, H, fRoughness);
    //float G = GeometrySmith(N, V, L, fRoughness);
    //float3 F = fresnelSchlick(HdotV, vSpecularColor);
    //float D = D_GGX(fRoughness, NdotH);
    //float G = G_GGX(fRoughness, NdotV, NdotL);
    //float3 F = F_Shlick(vSpecularColor, HdotV);
    
    float3 R = reflect(-V, N);
    
    float D = NormalDistributionGGXTR(NdotH, fRoughness);
    float G = GeometrySmith(NdotV, NdotL, fRoughness);
    float3 F = fresnelSchlick(HdotV, F0);
    
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= (1.0f - fMetallic);
    
    // Diffuse & Specular factors
    //float denom = max(4.0f * NdotV * NdotL, 0.001f); // 0.001f just in case product is 0
    float3 numerator = D * G * F;
    float denom = 4.0f * NdotV * NdotL;
    float3 specular_factor = numerator / max(denom, 0.001);
    
    float3 vRadiance = g_vLightDiffuse.xyz * EPSILON /*attenuation*/;
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
#endif