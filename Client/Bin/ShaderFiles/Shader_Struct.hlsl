#pragma pack(push, 4) // 4바이트 정렬

struct Fog_Desc
{
    float g_fFogStartDepth;
    float g_fFogStartDistance;
    float g_fFogDistanceValue;
    float g_fFogHeightValue;
    float g_fDistanceDensity;
    float g_fHeightDensity;
    bool bFog_Active;
};

#pragma pack(pop)
//struct Shader_Desc
//{
//	/* 활성여부 */
//    bool bHBAO_Plus_Active = { false };
//    bool bFog_Active = { false };
//    bool bHDR_Active = { false };
//    bool bFXAA_Active = { false };

//	/* HBAO+ */
//    float fRadius = 1.f;
//    float fBias = 0.1f;
//    float fPowerExponent = 2.f;
//    float fBlur_Sharpness = 16.f;

//	/* Fog */
//    float fFogStartDepth = 0.1f;
//    float fFogStartDistance = 10.f;
//    float fFogDistanceValue = 30.f;
//    float fFogHeightValue = 50.f;
//    float fFogDistanceDensity = 0.04f;
//    float fFogHeightDensity = 0.04f;
	
//	/* HDR */
//    float fmax_white = 0.4f;

//	/* Screen */
//    float fFinal_Saturation = 1.f;
//    float fFinal_Brightness = 1.f;

//};