#include "stdafx.h"
#include "Player_Weapon_Revolver.h"
#include "GameInstance.h"
#include "SMath.h"

CPlayer_Weapon_Revolver::CPlayer_Weapon_Revolver(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CWeapon_Player(pDevice, pContext, strPrototypeTag)
{

}

CPlayer_Weapon_Revolver::CPlayer_Weapon_Revolver(const CPlayer_Weapon_Revolver& rhs)
	:CWeapon_Player(rhs)
{
}

HRESULT CPlayer_Weapon_Revolver::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) 
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_Revolver::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	string path = "../Bin/DataFiles/Data_Weapon/Player/Revolver/Revolver.json";
	json In_Json;
	CJson_Utility::Load_Json(path.c_str(), In_Json);
	m_pTransformCom->Load_FromJson(In_Json);

	return S_OK;
}

void CPlayer_Weapon_Revolver::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer_Weapon_Revolver::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//if (m_pGameInstance->Key_Down(DIK_F))
	//{
	//	string path = "../Bin/DataFiles/Data_Weapon/Player/Revolver/Revolver.json";
	//	{
	//		json Out_Json;
	//		m_pTransformCom->Write_Json(Out_Json);
	//		CJson_Utility::Save_Json(path.c_str(), Out_Json);
	//	}
	//	//{
	//	//   json In_Json;
	//	//   CJson_Utility::Load_Json(path.c_str(), In_Json);
	//	//   m_pTransformCom->Load_FromJson(In_Json);
	//	//}
	//}
}

void CPlayer_Weapon_Revolver::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Weapon_Revolver::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon_Revolver::Fire(_float3 vTargetPos, CCharacter* pTarget)
{
	__super::Fire(L"Prototype_GameObject_Bullet_Revolver", LAYER_PLAYER_BULLET, vTargetPos, pTarget);
}

void CPlayer_Weapon_Revolver::Play_Weapon_Sound_UnHolster()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 6);
	switch (iRand)
	{
	case 0:
		strFileName = L"HM_Revolver_Unholster_01.wav";
		break;
	case 1:
		strFileName = L"HM_Revolver_Unholster_02.wav";
		break;
	case 2:
		strFileName = L"HM_Revolver_Unholster_03.wav";
		break;
	case 3:
		strFileName = L"HM_Revolver_Unholster_04.wav";
		break;
	case 4:
		strFileName = L"HM_Revolver_Unholster_05.wav";
		break;
	case 5:
		strFileName = L"HM_Revolver_Unholster_01.wav";
		break;
	default:
		strFileName = L"HM_Revolver_Unholster_01.wav";
		break;

	}
	m_pGameInstance->Play_Sound(L"PLAYER_WEAPON", strFileName, CHANNELID::SOUND_PLAYER_WEAPON, 10.f);

}

void CPlayer_Weapon_Revolver::Play_Weapon_Sound_Holster()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 4);
	switch (iRand)
	{
	case 0:
		strFileName = L"HM_Revolver_Holster_01.wav";
		break;
	case 1:
		strFileName = L"HM_Revolver_Holster_02.wav";
		break;
	case 2:
		strFileName = L"HM_Revolver_Holster_03.wav";
		break;
	case 3:
		strFileName = L"HM_Revolver_Holster_04.wav";
		break;
	default:
		strFileName = L"HM_Revolver_Holster_01.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_WEAPON", strFileName, CHANNELID::SOUND_PLAYER_WEAPON, 25.f);

}

void CPlayer_Weapon_Revolver::Play_Weapon_Sound_Aim()
{
}

void CPlayer_Weapon_Revolver::Play_Weapon_Sound_Reload()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 3);
	switch (iRand)
	{
	case 0:
		strFileName = L"Revolver_Reload_01.wav";
		break;
	case 1:
		strFileName = L"Revolver_Reload_02.wav";
		break;
	case 2:
		strFileName = L"Revolver_Reload_03.wav";
		break;
	default:
		strFileName = L"Revolver_Reload_01.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_WEAPON", strFileName, CHANNELID::SOUND_PLAYER_WEAPON, 10.f);
}

void CPlayer_Weapon_Revolver::Play_Weapon_Sound_Fire()
{
}

HRESULT CPlayer_Weapon_Revolver::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Player_Weapon_Revolver"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

CPlayer_Weapon_Revolver* CPlayer_Weapon_Revolver::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CPlayer_Weapon_Revolver* pInstance = new CPlayer_Weapon_Revolver(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Weapon_Revolver");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_Revolver::Clone(void* pArg)
{
	CPlayer_Weapon_Revolver* pInstance = new CPlayer_Weapon_Revolver(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Weapon_Revolver");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_Revolver::Pool()
{
	return new CPlayer_Weapon_Revolver(*this);
}

void CPlayer_Weapon_Revolver::Free()
{
	__super::Free();
}
