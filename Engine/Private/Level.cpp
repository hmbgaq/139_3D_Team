#include "..\Public\Level.h"
#include "GameInstance.h"

CLevel::CLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Tick(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

HRESULT CLevel::Set_ShaderOption(string filePath)
{
	json BasicJson = {};

	if (FAILED(CJson_Utility::Load_Json(filePath.c_str(), BasicJson)))
	{
		MSG_BOX("Fail to Load Shader");
		return E_FAIL;
	}

    /* 1. ¼ÎÀÌ´õ ÃÊ±âÈ­ */
    m_pGameInstance->Off_Shader();

    /* 2. ¼ÎÀÌ´õ ¼ÂÆÃ */
    PBR_DESC Desc_PBR = {};
    Desc_PBR.bPBR_ACTIVE                    = BasicJson["PBR"]["bPBR_Active"];
    Desc_PBR.fBrightnessOffset              = BasicJson["PBR"]["fPBR_Brightness"];
    Desc_PBR.fSaturationOffset              = BasicJson["PBR"]["fPBR_Saturation"];

    DEFERRED_DESC Desc_Deferred = {};
    Desc_Deferred.bRimBloom_Blur_Active     = BasicJson["Deferred"]["bRimBloom_Blur_Active"];
    Desc_Deferred.bShadow_Active            = BasicJson["Deferred"]["bShadow_Active"];

    HBAO_PLUS_DESC Desc_Hbao                = {};
    Desc_Hbao.bHBAO_Active                  = BasicJson["HBAO"]["bHBAO_Active"];
    Desc_Hbao.fBias                         = BasicJson["HBAO"]["fBias"];
    Desc_Hbao.fBlur_Sharpness               = BasicJson["HBAO"]["fBlur_Sharpness"];
    Desc_Hbao.fPowerExponent                = BasicJson["HBAO"]["fPowerExponent"];
    Desc_Hbao.fRadius                       = BasicJson["HBAO"]["fRadius"];

    FOG_DESC Desc_Fog = {};
    Desc_Fog.bFog_Active                    = BasicJson["Fog"]["bFog_Active"];	
    Desc_Fog.fFogDistanceDensity            = BasicJson["Fog"]["fFogDistanceDensity"];	
    Desc_Fog.fFogDistanceValue              = BasicJson["Fog"]["fFogDistanceValue"];
    Desc_Fog.fFogHeightDensity              = BasicJson["Fog"]["fFogHeightDensity"];
    Desc_Fog.fFogHeightValue                = BasicJson["Fog"]["fFogHeightValue"];	
    Desc_Fog.fFogStartDepth                 = BasicJson["Fog"]["fFogStartDepth"];
    Desc_Fog.fFogStartDistance              = BasicJson["Fog"]["fFogStartDistance"];
    Desc_Fog.vFogColor.x                    = BasicJson["Fog"]["vFogColor_x"];	
    Desc_Fog.vFogColor.y                    = BasicJson["Fog"]["vFogColor_y"];	
    Desc_Fog.vFogColor.z                    = BasicJson["Fog"]["vFogColor_z"];	
    Desc_Fog.vFogColor.w                    = BasicJson["Fog"]["vFogColor_w"];	

    RADIAL_DESC Desc_Radial = {};
    Desc_Radial.bRadial_Active              = BasicJson["Radial"]["bRadial_Active"]	;
    Desc_Radial.fRadial_Quality             = BasicJson["Radial"]["fRadial_Quality"];
    Desc_Radial.fRadial_Power               = BasicJson["Radial"]["fRadial_Power"]	;	

    DOF_DESC Desc_Dof = {};
    Desc_Dof.bDOF_Active                    = BasicJson["DOF"]["bDOF_Active"];
    Desc_Dof.DOF_Distance                   = BasicJson["DOF"]["fDOF_Distance"];
                                                                                     
    HDR_DESC Desc_HDR = {};                                                          
    Desc_HDR.bHDR_Active                    = BasicJson["HDR"]["bHDR_Active"];
    Desc_HDR.fmax_white                     = BasicJson["HDR"]["fmax_white"];
                                                                                     
    ANTI_DESC Desc_Anti = {};                                                        
    Desc_Anti.bFXAA_Active                  = BasicJson["Anti"]["bFXAA_Active"];
                                                                                     
    HSV_DESC Desc_HSV = {};                                                          
    Desc_HSV.bScreen_Active                 = BasicJson["HSV"]["bScreen_Active"];
    Desc_HSV.fFinal_Saturation              = BasicJson["HSV"]["fFinal_Saturation"];
    Desc_HSV.fFinal_Brightness              = BasicJson["HSV"]["fFinal_Brightness"];
                                                                                     
    VIGNETTE_DESC Desc_Vignette = {};                                                
    Desc_Vignette.bVignette_Active          = BasicJson["Vignette"]["bFinal_Active"];
    Desc_Vignette.fVignetteRatio            = BasicJson["Vignette"]["fFinal_Amount"];
    Desc_Vignette.fVignetteRadius           = BasicJson["Vignette"]["fFinal_CenterX"];	
    Desc_Vignette.fVignetteAmount           = BasicJson["Vignette"]["fFinal_CenterY"];	
    Desc_Vignette.fVignetteSlope            = BasicJson["Vignette"]["fFinal_Radius"];
    Desc_Vignette.fVignetteCenter_X         = BasicJson["Vignette"]["fFinal_Ratio"];
    Desc_Vignette.fVignetteCenter_Y         = BasicJson["Vignette"]["fFinal_Slope"];
                                                                                     
    SSR_DESC Desc_SSR = {};                                                          
    Desc_SSR.bSSR_Active                    = BasicJson["SSR"]["bFinal_Active"];
    Desc_SSR.fRayStep                       = BasicJson["SSR"]["fFinal_RayStep"];
    Desc_SSR.fStepCnt                       = BasicJson["SSR"]["fFinal_StepCnt"];
                                                                                     
    CHROMA_DESC	Desc_Chroma = {};                                                    
    Desc_Chroma.bChroma_Active              = BasicJson["Chroma"]["bFinal_Active"]; 
    Desc_Chroma.fChromaticIntensity         = BasicJson["Chroma"]["fFinal_Intensity"];


    LUMASHARPEN_DESC Desc_Luma = {};
    Desc_Luma.bLumaSharpen_Active           = BasicJson["Luma"]["bLuma_Active"];
    Desc_Luma.foffset_bias                  = BasicJson["Luma"]["fLuma_bias"];
    Desc_Luma.fsharp_clamp                  = BasicJson["Luma"]["fLuma_clamp"];
    Desc_Luma.fsharp_strength               = BasicJson["Luma"]["fLuma_strength"];
                                                                                      
    SCREENEFFECT_DESC Desc_ScreenEffect = {};                                         
    Desc_ScreenEffect.bGrayScale_Active     = BasicJson["Screen"]["GrayActive"]; 
    Desc_ScreenEffect.bSephia_Active        = BasicJson["Screen"]["SephiaActive"]; 
    Desc_ScreenEffect.GreyPower             = BasicJson["Screen"]["GreyPower"];
    Desc_ScreenEffect.SepiaPower            = BasicJson["Screen"]["SepiaPower"];
     
    // ¼ÎÀÌ´õ 14°³ 

    m_pGameInstance->Get_Renderer()->Set_PBR_Option(Desc_PBR);
    m_pGameInstance->Get_Renderer()->Set_Deferred_Option(Desc_Deferred);
    m_pGameInstance->Get_Renderer()->Set_HBAO_Option(Desc_Hbao);
    m_pGameInstance->Get_Renderer()->Set_Fog_Option(Desc_Fog);
    m_pGameInstance->Get_Renderer()->Set_RadialBlur_Option(Desc_Radial);
    m_pGameInstance->Get_Renderer()->Set_DOF_Option(Desc_Dof);
    m_pGameInstance->Get_Renderer()->Set_HDR_Option(Desc_HDR);
    m_pGameInstance->Get_Renderer()->Set_FXAA_Option(Desc_Anti);
    m_pGameInstance->Get_Renderer()->Set_HSV_Option(Desc_HSV);
    m_pGameInstance->Get_Renderer()->Set_Vignette_Option(Desc_Vignette);
    m_pGameInstance->Get_Renderer()->Set_SSR_Option(Desc_SSR);
    m_pGameInstance->Get_Renderer()->Set_Chroma_Option(Desc_Chroma);
    m_pGameInstance->Get_Renderer()->Set_ScreenEffect_Option(Desc_ScreenEffect);
    m_pGameInstance->Get_Renderer()->Set_LumaSharpen_Option(Desc_Luma);

	return S_OK;
}

void CLevel::Free()
{

 
 	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
