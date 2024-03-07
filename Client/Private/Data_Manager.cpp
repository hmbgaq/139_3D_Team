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

void CData_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
