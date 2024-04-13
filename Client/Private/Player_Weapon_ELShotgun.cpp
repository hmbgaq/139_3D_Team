#include "stdafx.h"
#include "Player_Weapon_ELShotgun.h"
#include "GameInstance.h"
#include "SMath.h"

CPlayer_Weapon_ELShotgun::CPlayer_Weapon_ELShotgun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CWeapon_Player(pDevice, pContext, strPrototypeTag)
{

}

CPlayer_Weapon_ELShotgun::CPlayer_Weapon_ELShotgun(const CPlayer_Weapon_ELShotgun& rhs)
	: CWeapon_Player(rhs)
{
}

HRESULT CPlayer_Weapon_ELShotgun::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_ELShotgun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	string path = "../Bin/DataFiles/Data_Weapon/Player/ELShotgun/ELShotgun.json";
	json In_Json;
	CJson_Utility::Load_Json(path.c_str(), In_Json);
	m_pTransformCom->Load_FromJson(In_Json);

	return S_OK;
}

void CPlayer_Weapon_ELShotgun::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer_Weapon_ELShotgun::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CPlayer_Weapon_ELShotgun::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Weapon_ELShotgun::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}


void CPlayer_Weapon_ELShotgun::Play_Weapon_Sound_Reload()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 4);
	switch (iRand)
	{
	case 0:
		strFileName = L"Shotgun_Reload_01.wav";
		break;
	case 1:
		strFileName = L"Shotgun_Reload_02.wav";
		break;
	case 2:
		strFileName = L"Shotgun_Reload_03.wav";
		break;
	case 3:
		strFileName = L"Shotgun_Reload_04.wav";
		break;
	default:
		strFileName = L"Shotgun_Reload_01.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_WEAPON", strFileName, CHANNELID::SOUND_PLAYER_WEAPON, 10.f);


}

void CPlayer_Weapon_ELShotgun::Play_Weapon_Sound_Fire()
{
	
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 2);
	switch (iRand)
	{
	case 0:
		strFileName = L"Shotgun_Fire_Player_01.wav";
		break;
	case 1:
		strFileName = L"Shotgun_Fire_Player_02.wav";
		break;
	default:
		strFileName = L"Shotgun_Fire_Player_01.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_ATTACK", strFileName, CHANNELID::SOUND_PLAYER_ATTACK, 10.f);

}

HRESULT CPlayer_Weapon_ELShotgun::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Player_Weapon_ELShotgun"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

void CPlayer_Weapon_ELShotgun::Fire(_float3 vTargetPos, CCharacter* pTarget)
{
	for (_uint i = 0; i < 4; ++i)
	{
		__super::Fire(L"Prototype_GameObject_Bullet_ELShotgun", LAYER_PLAYER_BULLET, vTargetPos, pTarget);
	}

} 

CPlayer_Weapon_ELShotgun* CPlayer_Weapon_ELShotgun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CPlayer_Weapon_ELShotgun* pInstance = new CPlayer_Weapon_ELShotgun(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Weapon_ELShotgun");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_ELShotgun::Clone(void* pArg)
{
	CPlayer_Weapon_ELShotgun* pInstance = new CPlayer_Weapon_ELShotgun(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Weapon_ELShotgun");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_ELShotgun::Pool()
{
	return  new CPlayer_Weapon_ELShotgun(*this);
}

void CPlayer_Weapon_ELShotgun::Free()
{
	__super::Free();
}
