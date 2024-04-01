#ifndef SHADER_DEFINES_HLSLI
#include "Shader_Defines.hlsli"
#define SHADER_DEFINES_HLSLI
#endif

/* ----------------- Variable ----------------- */
float4      g_SunColor = { 1.f, 1.f, 1.f, 0.f };
TextureCube g_Irradiance;
TextureCube g_PreFiltered;
Texture2D   g_BRDF_Texture;
/*=============================================================
 
                            D, G, F
                                
==============================================================*/

// 1. Normal Distribution Function (NDF)
// half way vector h 와 일치하는 microfacets의 표면 영역의 근사치를 구한다. 
// roughness가 0에 가까우면 매우 작은 반경에 집중되어 halfway vector와 일치하는 microfacets의 근사치를 구한다. -> 매우 밝은 색상을 띄게된다. 
// 1에 가까우면 더 많이 halfway vector와 일치하겠지만 자긍ㄴ 한 반경에 집중되지않아서 어느 특정부부남ㄴ 엄청 밝은색을 띄는게 아니라 전체적으로 회색 색상을 띄게된다. 
float My_NormalDistributionGGXTR(float3 n, float3 h, float a)
{
    float a2 = a * a;
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}


// 2. Geometry Function 
// Roughness를 인자로 받아서 표면이 서로 겹쳐서 빛을 차단하는 정도의 근사치를 통계적으로 구한다. 
// k는 direct lighting이냐 IBL Lighting이냐에 따라서 달라질 수 있다. 
float My_GeometrySchlickGGX(float NdotV, float roughness)  // k is a remapping of roughness based on direct lighting or IBL lighting
{
    // k : roughness값의 remapping (direct light, IBL 등 경우에 따라 다름)
    // 그리고 아웃풋은 얼마나 미세면그림자가 져있는지가 나온다. 
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f; // direct

    //float k = (roughness * roughness) / 2.0f; // Ibl

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}
  
float My_GeometrySmith(float3 n, float3 v, float3 l, float k)
{
    //  Geometry Obstruction
    float NdotV = saturate(dot(n, v));
    //  Geometry Shadowing
    float NdotL = saturate(dot(n, l));
    
    //float k = (roughness * roughness) / 2.0f; // IBL 

    float ggx2 = My_GeometrySchlickGGX(NdotV, k);
    float ggx1 = My_GeometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}

// 3. Fresnel Equation - 프레넬방정식
// 우리가 보는 각도에 따른 반사율과 굴절율을 계산한다. 
// -> 매우 복잡한 방정식을 가지고 있으나 슐릭(Schlick)의 근사식으로 간단하게 구할 수 있다.
float3 My_fresnelSchlick(float cosTheta, float3 F0)
{
    // 가까운물은 바닥까지 잘 보이고 먼 물은 거울처럼 반사되어 보이는것
    // 빛은 다른 매질과 충돌할 떄 일정 비율은 굴절되고 일정 비율은 반사가 되는데 
    // 입사각에 따라 반사, 굴절의 정도가 달라지는 현상을 표현한것이 프레넬 방정식이다. 
    // 직접 계산하기에는 매우 복잡해서 PBR에서는 근사화한 버전을 사용한다 = 슐릭의 근사식
   
    // costheta는 (half way 벡터 * view벡터)로 쓰이는값이다. 
    return (F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f));
}

/*=============================================================
 
                             IBL 
                                
==============================================================*/
float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

/*=============================================================
 
                             BRDF 
                                
==============================================================*/

// Result = BRDF / 양방향 반사분포함수 
// 인자 두개를 받고 그 관계에 따라 반사율 (반사광 / 입사광)을 출력하는 함수. 
// Mtrl의 반사와 굴절 속성의 근사치를 micofacet이론을 바타응로 구한다. 
// 대부분 실시간 렌더링파이프라인에서는 Cook-Torrance BRDF를 사용한다. 

float3 MY_BRDF(in float fRoughness, in float fMetallic, in float3 vAlbedo, in float3 F0, in float3 N, in float3 V, in float3 L, in float3 H)
{
    //Result 
    float3 vColor = { 1.f, 1.f, 1.f };
    
    //  calculate light radiance
    //float3 lightDir = -LightDirection;                    // Deferred에서 L구할때 이미 계산함
    //float3 halfwayVec = normalize(viewDir + lightDir);    // Deferred에서 H 구할떄 이미 계산함
    float3 radiance = g_SunColor.rgb;

    //const float NdotL = saturate(dot(N, L));
    //const float NdotV = saturate(dot(N, V));
    //const float NdotH = saturate(dot(N, H));
    const float HdotV = saturate(dot(H, V));
    
    //  Cook-Torrance specular BRDF
    // EpicGame's Unreal Engine4 가 채용중인 PBR : Trowbridge-Reitz GGX(D), Fresnel-Schlick(F), 그리고 Smith's Schlick-GGX(G)
    // pbr에서 사용되는 D, F, G 는 여러가지 버전이 있을 수 있다. 계산속도를 중시한다던가 물리적으로 사실적인가 등을 고려하기 떄문이다. 
    float   D = My_NormalDistributionGGXTR(N, H, fRoughness); // roughness값에 의한 반사분포
    float   G = My_GeometrySmith(N, V, L, fRoughness); // 미세면 그림자
    float3  F = My_fresnelSchlick(HdotV, F0); // 다른 각도로 봤을 떄 달라지는 반사정도 - 금석성과 관련이 크다.  costheta는 (half way 벡터 * view벡터)로 쓰이는값이다. 
    
    float3 nominator = D * G * F;

    //  Wo : View Direction
    //  Wi : Light Direction
    float WoDotN = saturate(dot(V, N));
    float WiDotN = saturate(dot(L, N));
    float denominator = (4 * WoDotN * WiDotN);

    // 0.001f just in case product is 0
    //float3 specular_factor = nominator / (denominator + 0.001f);
    float3 specular_factor = nominator / (denominator + 0.001f); //max(denominator, 0.001);

    //  Energy Conservation
    float3 kS = F; //  reflection energy
    float3 kD = 1.0f - kS; //  refraction energy
    kD *= 1.0 - fMetallic; //  if metallic nullify kD

    //  Calculate Cook-Torrance Reflectance Equation 
    vColor = (((kD * vAlbedo / PI) + specular_factor) * radiance * WiDotN);
    // (kD * vDiffuseColor / PI) = Cook-Torrance Diffuse
    // specular_factor = BRDF수행하고 추가연산한 결과 
    // radiance = 카메라에 SOLID ANGLE을 가진 형태로 들어오는 빛의 양 -> 법선벡터와 빛의 반대방향의 dot
    // WiDotN
    
    return vColor;
}


float3 MY_BRDF_Irradiance(in float fRoughness, in float fMetallic, in float3 vAlbedo,
in float3 F0, in float fAO,
in float3 N, in float3 V, in float3 L, in float3 H)
{
    //Result 
    float3 vColor = { 1.f, 1.f, 1.f };
    
    //  calculate light radiance
    //float3 lightDir = -LightDirection;                    // Deferred에서 L구할때 이미 계산함
    //float3 halfwayVec = normalize(viewDir + lightDir);    // Deferred에서 H 구할떄 이미 계산함
    float3 radiance = g_SunColor.rgb;

    //const float NdotL = saturate(dot(N, L));
    //const float NdotV = saturate(dot(N, V));
    //const float NdotH = saturate(dot(N, H));
    const float HdotV = saturate(dot(H, V));
    
    //  Cook-Torrance specular BRDF
    // EpicGame's Unreal Engine4 가 채용중인 PBR : Trowbridge-Reitz GGX(D), Fresnel-Schlick(F), 그리고 Smith's Schlick-GGX(G)
    // pbr에서 사용되는 D, F, G 는 여러가지 버전이 있을 수 있다. 계산속도를 중시한다던가 물리적으로 사실적인가 등을 고려하기 떄문이다. 
    float D = My_NormalDistributionGGXTR(N, H, fRoughness); // roughness값에 의한 반사분포
    float G = My_GeometrySmith(N, V, L, fRoughness); // 미세면 그림자
    float3 F = My_fresnelSchlick(HdotV, F0); // 다른 각도로 봤을 떄 달라지는 반사정도 - 금석성과 관련이 크다.  costheta는 (half way 벡터 * view벡터)로 쓰이는값이다. 
    
    float3 nominator = D * G * F;

    //  Wo : View Direction
    //  Wi : Light Direction
    float WoDotN = saturate(dot(V, N));
    float WiDotN = saturate(dot(L, N));
    float denominator = (4 * WoDotN * WiDotN);

    // 0.001f just in case product is 0
    //float3 specular_factor = nominator / (denominator + 0.001f);
    float3 specular_factor = nominator / (denominator + 0.001f); //max(denominator, 0.001);

    //  Energy Conservation
    float3 kS = F; //  reflection energy
    float3 kD = 1.0f - kS; //  refraction energy
    kD *= 1.0 - fMetallic; //  if metallic nullify kD

    //  Calculate Cook-Torrance Reflectance Equation 
    float3 Lo = (((kD * vAlbedo / PI) + specular_factor) * radiance * WiDotN);
    // (kD * vDiffuseColor / PI) = Cook-Torrance Diffuse
    // specular_factor = BRDF수행하고 추가연산한 결과 
    // radiance = 카메라에 SOLID ANGLE을 가진 형태로 들어오는 빛의 양 -> 법선벡터와 빛의 반대방향의 dot
    // WiDotN
    
    /* ---- MY_BRDF_Irradiance Map 을 PBR에 적용 ---- */
    kS = FresnelSchlickRoughness(max(dot(N, V), 0.8f), F0, fRoughness);
    kD = float3(1.f, 1.f, 1.f) -kS;
    kD *= 1.0 - fMetallic;
    
    float3 Irradiance = g_Irradiance.Sample(ClampSampler, N).rgb;
    
    float3 Diffuse = vAlbedo * Irradiance;
    //float3 ambient = (kD * Diffuse) * fAO;
    
    const float MAX_REF_LOD = 4.0f;
    
    float3 R = reflect(-V, N);
    
    float3 prefiltered = g_PreFiltered.SampleLevel(ClampSampler, R, fRoughness * MAX_REF_LOD).rgb;
    float2 brdf = g_BRDF_Texture.Sample(ClampSampler, float2(max(dot(N, V), 0.0f), fRoughness)).rg;
    float3 specular = prefiltered * (kS * brdf.x + brdf.y);

    float3 ambient = (kD * Diffuse + specular) * fAO;
    vColor = ambient + Lo;
    
    return vColor;
}
