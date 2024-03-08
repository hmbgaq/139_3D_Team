#include "stdafx.h"
#include "Data_Manager.h"

#include "GameInstance.h"

#include "Player.h"
#include "Camera_Dynamic.h"
#include "MasterCamera.h"

IMPLEMENT_SINGLETON(CData_Manager);

CData_Manager::CData_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CData_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	Safe_AddRef(m_pDevice);

	m_pContext = pContext;
	Safe_AddRef(m_pContext);

	PlayerInfo_Setting();

	return S_OK;
}

void CData_Manager::Set_Player(CPlayer* _pPlayer)
{
	m_pPlayer = _pPlayer;
}

CPlayer* CData_Manager::Get_Player()
{
	return m_pPlayer;
}

void CData_Manager::Reset_Player(LEVEL eLEVEL)
{
	// Example
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	_float3 vPos = { 0.f, 0.f, 0.f };

	switch (eLEVEL)
	{
	case Client::LEVEL_GAMEPLAY:
		//vPos = { 0.f, 0.f, 0.f };
		break;
	case Client::LEVEL_SNOWMOUNTAIN:
		//vPos = { 0.f, 0.f, 0.f };
		break;
	case Client::LEVEL_LAVA:
		//vPos = { 0.f, 0.f, 0.f };
		break;
	case Client::LEVEL_TOOL:
		//vPos = { 0.f, 0.f, 0.f };
		break;
	default:
		break;
	}

	m_pPlayer->Initialize(&GameObjectDesc);
	m_pPlayer->Set_Position(vPos);

}

void CData_Manager::Set_MasterCamera(CMasterCamera* _pMasterCamera)
{
	m_pMasterCamera = _pMasterCamera;
}

CMasterCamera* CData_Manager::Get_MasterCamera()
{
	return m_pMasterCamera;
}

void CData_Manager::Reset_MasterCamera(LEVEL eLEVEL)
{
	CCamera_Dynamic::DYNAMIC_CAMERA_DESC tDesc = {};
	tDesc.fMouseSensor = 0.05f;
	tDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	tDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	tDesc.fFovy = XMConvertToRadians(60.0f);
	tDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	tDesc.fNear = 0.1f;
	tDesc.fFar = m_pGameInstance->Get_CamFar();
	tDesc.fSpeedPerSec = 5.f;
	tDesc.fRotationPerSec = XMConvertToRadians(180.0f);

	_float3 vPos = { 0.f, 0.f, 0.f };

	switch (eLEVEL)
	{
	case Client::LEVEL_GAMEPLAY:
		//vPos = { 0.f, 0.f, 0.f };
		break;
	case Client::LEVEL_SNOWMOUNTAIN:
		//vPos = { 0.f, 0.f, 0.f };
		break;
	case Client::LEVEL_LAVA:
		//vPos = { 0.f, 0.f, 0.f };
		break;
	case Client::LEVEL_TOOL:
		//vPos = { 0.f, 0.f, 0.f };
		break;
	default:
		break;
	}

	m_pMasterCamera->Initialize(&tDesc);
	m_pMasterCamera->Set_Position(vPos);
}

#pragma region SH_ADD
// Player_Setting
void CData_Manager::PlayerInfo_Setting()
{
	/* HP */
	m_fMaxHP = 100.0f;
	m_fCurHP = m_fMaxHP;

	/* EXP */
	m_fMaxEXP = 100.0f;
	m_fCurEXP = 0.0f;

	/* SKILL_GUIGE */
	m_fMaxSkillGuige = 100.0f;
	m_fCurSkillGuige = /*0.0f*/m_fMaxSkillGuige;
}

// Player_HP
void CData_Manager::Limit_HP()
{
	/* 1. 모든 체력 소진 */
	if (m_fCurHP <= 0.f)
	{
		/* Dead */
		m_fCurHP = 0.f;
	}

	/* 2. 현재 체력 -> 맥스 체력 초과 */
	if (m_fCurHP >= m_fMaxHP)
	{
		m_fCurHP = m_fMaxHP;
	}
}

// Player_EXP(Limit)
_bool CData_Manager::Limit_EXP()
{
	/* 1. 경험치 하한선 방지 */
	if (m_fCurEXP <= 0.f)
	{
		m_fCurEXP = 0.f;
	}

	/* 2. 현재 경험치 최대치 도달 (레벨 업) */
	if (m_fCurEXP >= m_fMaxEXP)
	{
		m_fCurHP = m_fMaxHP;
		m_bLevelUp = true;

		return true; // Level UP
	}

	return false; // Not Event
}

// Player_SkillGuige
void CData_Manager::Limit_SkillGuige()
{
}

// Player_Level
void CData_Manager::Limit_Level()
{
	/* 1. 레벨 업 */
	if (m_bLevelUp)
	{
		++m_iCurLevel; // Add Level
		m_bLevelUp = false;
	}
}

// Limit_Manager
void CData_Manager::Limit_Manager()
{
	//=>HP
						Limit_HP();
	//=>EXP
						Limit_EXP();
	//=>SkillGuige
						Limit_SkillGuige();
	//=>Level
						Limit_Level();
}
#pragma endregion SH_END

void CData_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
