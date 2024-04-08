#include "stdafx.h"
#include "Player_Weapon_ELShotgun.h"
#include "GameInstance.h"

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

	//if (m_pGameInstance->Key_Down(DIK_F))
	//{
	//	string path = "../Bin/DataFiles/Data_Weapon/Player/ELShotgun/ELShotgun.json";
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
