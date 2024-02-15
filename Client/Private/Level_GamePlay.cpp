#include "stdafx.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Dynamic.h"
#include "Environment_Instance.h"
#include "Effect_Instance.h"

#pragma region UI
#include "UI_MonsterHp.h"
#include "UI_MonsterHpFrame.h"
#pragma endregion

#include "LandObject.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	FAILED_CHECK(Ready_LightDesc());
	FAILED_CHECK(Ready_Layer_Camera(TEXT("Layer_Camera")));
	FAILED_CHECK(Ready_Layer_Effect(TEXT("Layer_Effect")));
	FAILED_CHECK(Ready_Layer_BackGround(TEXT("Layer_BackGround")));
	FAILED_CHECK(Ready_LandObjects());
	FAILED_CHECK(Ready_Layer_Test(TEXT("Layer_Test")));

	if (FAILED(Ready_UI()))
		return E_FAIL;

	

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LightDesc()
{
	LIGHT_DESC			LightDesc{};
	{
		LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
		LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
		LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
		LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
		LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

		FAILED_CHECK(m_pGameInstance->Add_Light(LightDesc, TempLightNumber));
	}
	{
		ZeroMemory(&LightDesc, sizeof LightDesc);

		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(30.f, 3.f, 30.f, 1.f);
		LightDesc.fRange = 20.f;
		LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.0f, 1.f);
		LightDesc.vAmbient = _float4(0.4f, 0.1f, 0.1f, 1.f);
		LightDesc.vSpecular = LightDesc.vDiffuse;
		FAILED_CHECK(m_pGameInstance->Add_Light(LightDesc, TempLightNumber));

		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(50.f, 3.f, 30.f, 1.f);
		LightDesc.fRange = 20.f;
		LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.0f, 1.f);
		LightDesc.vAmbient = _float4(0.1f, 0.4f, 0.1f, 1.f);
		LightDesc.vSpecular = LightDesc.vDiffuse;
		FAILED_CHECK(m_pGameInstance->Add_Light(LightDesc, TempLightNumber));

		LightDesc.eType = LIGHT_DESC::TYPE_POINT;
		LightDesc.vPosition = _float4(70.f, 10.f, 30.f, 1.f);
		LightDesc.fRange = 20.f;
		LightDesc.vDiffuse = _float4(1.f, 0.0f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(0.4f, 0.1f, 0.4f, 1.f);
		LightDesc.vSpecular = LightDesc.vDiffuse;
		FAILED_CHECK(m_pGameInstance->Add_Light(LightDesc, TempLightNumber));
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
{
	CCamera_Dynamic::DYNAMIC_CAMERA_DESC		Desc = {};

	Desc.fMouseSensor = 0.05f;
	Desc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = m_pGameInstance->Get_CamFar();
	Desc.fSpeedPerSec = 15.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"), &Desc));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring & strLayerTag, void* pArg)
{
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"), pArg));

	//CGameObject* pPlayer = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"), pArg);
	//if (nullptr == pPlayer)
	//	return E_FAIL;

	//m_pGameInstance->Set_Player(pPlayer);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
{
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Blue")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Red")));
	//FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Effect_Explosion")));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring & strLayerTag, void* pArg)
{
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), pArg));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain")));
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky")));

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Skydome"))))
	//	return E_FAIL;

	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	CLandObject::LANDOBJECT_DESC	LandObjectDesc{};

	LandObjectDesc.pTerrainBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
	LandObjectDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform")));

	FAILED_CHECK(Ready_Layer_Player(TEXT("Layer_Player"), &LandObjectDesc));
	FAILED_CHECK(Ready_Layer_Monster(TEXT("Layer_Monster"), &LandObjectDesc));
	FAILED_CHECK(Ready_Layer_Building(TEXT("Layer_Building"), &LandObjectDesc));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Building(const wstring & strLayerTag, void* pArg)
{

	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_ForkLift"), pArg));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Test(const wstring& strLayerTag)
{
	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Interact_Chain")));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_UI()
{
	FAILED_CHECK(Ready_Layer_UI_Monster(TEXT("Layer_UI_Monster"), nullptr));

	FAILED_CHECK(Ready_Layer_UI_Monster(TEXT("Layer_UI_Player"), nullptr));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_Monster(const wstring& strLayerTag, void* pArg)
{
	/* 추 후 파싱해서 정보 받아오기 */
	
	// TEST
	CUI_MonsterHp::MONSTER_HP tMonsterHp;
	CUI_MonsterHpFrame::MONSTER_FRAME tMonsterFrame;
	
	tMonsterHp.fX = 50.f;
	tMonsterHp.fY = 50.f;
	tMonsterHp.fSizeX = 30.f;
	tMonsterHp.fSizeY = 30.f;

	tMonsterFrame.fX = 500.f;
	tMonsterFrame.fY = 300.f;
	tMonsterFrame.fSizeX = 100.f;
	tMonsterFrame.fSizeY = 100.f;
	tMonsterFrame.eMonsterType = CUI_MonsterHpFrame::SMALL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_MonsterHpFrame"), &tMonsterFrame)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_UI_MonsterHp"), &tMonsterHp)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI_Player(const wstring& strLayerTag, void* pArg)
{
	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)

{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
