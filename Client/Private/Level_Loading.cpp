#include "stdafx.h"
#include "..\Public\Level_Loading.h"
#include "Loader.h"

#include "GameInstance.h"
#include "Level_Logo.h"
#include "Level_Intro.h"
#include "Level_IntroBoss.h"
#include "Level_GamePlay.h"
#include "Level_SnowMountain.h"
#include "Level_SnowMountainBoss.h"
#include "Level_Tool.h"
#include "UI_Manager.h"

CLevel_Loading::CLevel_Loading(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
	, m_pUI_Manager(CUI_Manager::GetInstance())
{
	Safe_AddRef(m_pUI_Manager);
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	/* 추후 로딩 레벨이 끝나면 원래 목적으로 뒀던 레벨로 넘어가기위해서. */
	m_eNextLevelID = eNextLevelID;
	m_pGameInstance->Set_CurrentLevel(m_eNextLevelID);

	/*m_iNextLevel = m_eNextLevelID;*/
	/* 메인스레드로 대충 로드한다. */
	/* 로딩용 자원을 로드한다. */
	/* 로딩레벨에서 보여줘야할 객체들을 생성한다.(배경, 일러스트, 로딩바) */

	/* 추가적인 스레드를 생성하여 eNextLevelID에 필요한 자원들을 로드한다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);

	/* Loading BackGround */
	switch (m_eNextLevelID)
	{
	case Client::LEVEL_STATIC:
		break;
	case Client::LEVEL_LOGO:
		//m_pUI_Manager->Ready_Loading_MainMenu(LEVEL_LOGO);
		break;
	case Client::LEVEL_INTRO:
		//m_pUI_Manager->Ready_Loading_Intro(LEVEL_INTRO); // Loading UI 생성
		//m_pUI_Manager->Active_Loading_Intro(true);			 // UI ON
		break;
	case Client::LEVEL_INTRO_BOSS:
		m_pUI_Manager->Ready_Loading_IntroBoss(LEVEL_INTRO);	 // Loading UI 생성
		m_pUI_Manager->Active_Loading_IntroBoss(true);			 // UI ON
		m_pUI_Manager->NonActive_LeftHUD();
		m_pUI_Manager->NonActive_RightHUD();
		break;
	case Client::LEVEL_SNOWMOUNTAIN:
		//m_pUI_Manager->Ready_Loading_SnowMountain(LEVEL_SNOWMOUNTAIN);
		break;
	case Client::LEVEL_SNOWMOUNTAINBOSS:
		break;
	case Client::LEVEL_LAVA:
		break;
	case Client::LEVEL_TOOL:
		break;
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_GAMEPLAY:
		m_pUI_Manager->Ready_Loading_Intro(LEVEL_INTRO);	 // Loading UI 생성
		m_pUI_Manager->Active_Loading_Intro(true);			 // UI ON
		break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	if (true == m_pLoader->isFinished())
	{
		//if (GetKeyState(VK_RETURN) & 0x8000)
		{
			CLevel*		pNewLevel = { nullptr };

			switch (m_eNextLevelID)
			{
			case LEVEL_LOGO:
				
				pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_INTRO:
				pNewLevel = CLevel_Intro::Create(m_pDevice, m_pContext);
				//m_pUI_Manager->Active_Loading_Intro(false);			 // UI OFF
				break;
			case LEVEL_INTRO_BOSS:
				pNewLevel = CLevel_IntroBoss::Create(m_pDevice, m_pContext);
				m_pUI_Manager->Active_LeftHUD();
				m_pUI_Manager->Active_RightHUD();
				m_pUI_Manager->Active_Loading_IntroBoss(false);			 // UI ON
				break;
			case LEVEL_SNOWMOUNTAIN:
				pNewLevel = CLevel_SnowMountain::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_SNOWMOUNTAINBOSS:
				pNewLevel = CLevel_SnowMountainBoss::Create(m_pDevice, m_pContext);
				break;
			case LEVEL_GAMEPLAY:
				pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				m_pUI_Manager->Active_Loading_Intro(false);			 // UI ON
				break;
			case LEVEL_TOOL:
				pNewLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
				break;
			}

			if (nullptr == pNewLevel)
				return;

			if (FAILED(m_pGameInstance->Open_Level(m_eNextLevelID, pNewLevel)))
				return;
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Print_LoadingText();

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
	Safe_Release(m_pUI_Manager);

}
