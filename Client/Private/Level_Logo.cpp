#include "stdafx.h"
#include "Level_Logo.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Level_Loading.h"
#include "UI_Manager.h"
#include "Data_Manager.h"


#include "Effect_Manager.h"

//#include <mfapi.h>
//#include <mfreadwrite.h>

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
	, m_pUIManager(CUI_Manager::GetInstance())
{
	Safe_AddRef(m_pUIManager);
}

HRESULT CLevel_Logo::Initialize()
{
	//FAILED_CHECK(Ready_Layer_BackGround(TEXT("Layer_BackGround")));

	Set_Filter();
	
	m_pDataManager = CData_Manager::GetInstance();

	FAILED_CHECK(Ready_Static_UI());


	FAILED_CHECK(m_pUIManager->Ready_MainMenu(LEVEL_LOGO));
	m_pUIManager->Active_MainMenu();

	FAILED_CHECK(m_pUIManager->Ready_MouseCursor(LEVEL_STATIC)); // 생성 시점을 바꿔보자.
	m_pUIManager->NonActive_MouseCursor(); // 마우스 커서 UI를 끄면 메인화면 스타트가 꺼짐..
	
	m_pUIManager->NonActive_MainList();
	m_pUIManager->NonActive_LevelList();
	m_pUIManager->NonActive_MainLogo();
	m_pDataManager->Set_GameState(GAME_STATE::UI);
	ShowCursor(false);


	FAILED_CHECK(EFFECT_MANAGER->Ready_EffectPool()); // 이펙트 풀

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_TAB))
	{
		_int iCheckPoint = 0;
		iCheckPoint = MessageBox(g_hWnd, L"확인 선택 시 Intro(테스트맵), 취소 선택 시 되돌아가기.", L"Intro(테스트) 맵으로", MB_OKCANCEL);
	
		// 확인 버튼을 눌렀을 때
		if (iCheckPoint == IDOK)
		{
			m_pDataManager->Set_SelectLevel(LEVEL_INTRO);
		}
		else if (iCheckPoint == IDCANCEL)
		{

		}
	}

	if (m_pDataManager->Get_SelectLevel() != LEVEL_LOGO &&
		m_pDataManager->Get_SelectLevel() != LEVEL_END &&
		m_pDataManager->Get_SelectLevel() != LEVEL_TOOL)
	{
		FAILED_CHECK_RETURN(Ready_Static_UI(), );
	}

	switch (m_pDataManager->Get_SelectLevel())
	{
		case Client::LEVEL_STATIC:
			break;
		case Client::LEVEL_LOGO:
			break;
		case Client::LEVEL_INTRO:
			FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_INTRO)), );
			break;
		case Client::LEVEL_INTRO_BOSS:
			FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_INTRO_BOSS)),);
			break;
		case Client::LEVEL_SNOWMOUNTAIN:
			FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SNOWMOUNTAIN)), );
			break;
		case Client::LEVEL_SNOWMOUNTAINBOSS:
			FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SNOWMOUNTAINBOSS)), );
			break;
		case Client::LEVEL_LAVA:
			break;
		case Client::LEVEL_TOOL:
			FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TOOL)), );
			break;
		case Client::LEVEL_LOADING:
			break;
		case Client::LEVEL_GAMEPLAY:
			FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY)), );
			break;
		case Client::LEVEL_END:
			break;
		default:
			break;
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("메인화면입니다."));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Static_UI()
{
	if (m_bUI_ReadyOK == false)
	{
		// Ready Interface
		FAILED_CHECK(m_pUIManager->Ready_Interface(LEVEL_STATIC));
		// Ready Crosshair
		FAILED_CHECK(m_pUIManager->Ready_Crosshair(LEVEL_STATIC));
		m_pUIManager->NonActive_Crosshair();
		// Ready DiedScreen
		FAILED_CHECK(m_pUIManager->Ready_DiedScreen(LEVEL_STATIC));
		// Ready MouseCursor
		//FAILED_CHECK(m_pUIManager->Ready_MouseCursor(LEVEL_STATIC));
		// Ready SkillWindow
		FAILED_CHECK(CUI_Manager::GetInstance()->Ready_SkillWindow(LEVEL_STATIC));
		m_pUIManager->NonActive_SkillWindowAll();
		// Ready OptionWindow
		FAILED_CHECK(m_pUIManager->Ready_Option(LEVEL_STATIC));
		// Ready HitUI
		FAILED_CHECK(m_pUIManager->Ready_HitUI(LEVEL_STATIC));
		m_pUIManager->NonActive_HitUI();
		// Ready LetterBox
		FAILED_CHECK(m_pUIManager->Ready_LetterBox(LEVEL_STATIC));
		m_pUIManager->NonActive_LetterBox();

		m_pUIManager->NonActive_UI(); // Basic UI All
		//m_pUIManager->Active_MouseCursor();
		m_bUI_ReadyOK = true;
	}

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	CBackGround::BACKGROUND_DESC		BackGroundDesc = {};

	BackGroundDesc.fX = g_iWinSizeX >> 1;
	BackGroundDesc.fY = g_iWinSizeY >> 1;
	BackGroundDesc.fSizeX = g_iWinSizeX;
	BackGroundDesc.fSizeY = g_iWinSizeY;
	BackGroundDesc.fRotationPerSec = XMConvertToRadians(90.f);
	BackGroundDesc.fSpeedPerSec = 10.f;

	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_BackGround"), &BackGroundDesc));

	return S_OK;
}

void CLevel_Logo::Set_Filter()
{
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::PLAYER_PARRYING, (_uint)COLLISION_LAYER::MONSTER_ATTACK);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::PLAYER_ATTACK, (_uint)COLLISION_LAYER::MONSTER_SHIELD);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::PLAYER_ATTACK, (_uint)COLLISION_LAYER::WEAKNESS);

	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::PLAYER_ATTACK, (_uint)COLLISION_LAYER::MONSTER);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::MONSTER_ATTACK, (_uint)COLLISION_LAYER::PLAYER);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::PLAYER, (_uint)COLLISION_LAYER::MONSTER);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::MONSTER, (_uint)COLLISION_LAYER::MONSTER);



	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::DESTRUCTABLE_PROPS, (_uint)COLLISION_LAYER::MONSTER_ATTACK);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::DESTRUCTABLE_PROPS, (_uint)COLLISION_LAYER::PLAYER_ATTACK);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::DESTRUCTABLE_PROPS, (_uint)COLLISION_LAYER::EXPLOSION_ATTACK);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::DESTRUCTABLE_PROPS, (_uint)COLLISION_LAYER::PLAYER);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::DESTRUCTABLE_PROPS, (_uint)COLLISION_LAYER::MONSTER);

	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::EXPLOSION_ATTACK, (_uint)COLLISION_LAYER::PLAYER);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::EXPLOSION_ATTACK, (_uint)COLLISION_LAYER::MONSTER);


	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::OBSTACLE, (_uint)COLLISION_LAYER::PLAYER);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::OBSTACLE, (_uint)COLLISION_LAYER::MONSTER);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::OBSTACLE, (_uint)COLLISION_LAYER::PLAYER_ATTACK);

	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::INTERACT, (_uint)COLLISION_LAYER::INTERACT);
	m_pGameInstance->Check_Group((_uint)COLLISION_LAYER::INTERACT, (_uint)COLLISION_LAYER::PLAYER);



	//Obstacle





	//m_pGameInstance->Check_PhysXFilterGroup((_uint)PHYSX_COLLISION_LAYER::PLAYER, (_uint)PHYSX_COLLISION_LAYER::GROUND);
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

	/* UIManager Delete */
	if (m_pUIManager)
		Safe_Release(m_pUIManager);
}
