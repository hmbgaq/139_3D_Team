#include "stdafx.h"
#include "Weapon_Edgar.h"
#include "GameInstance.h"
#include "MasterCamera.h"
#include "SpringCamera.h"
#include "Data_Manager.h"

CWeapon_Edgar::CWeapon_Edgar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CWeapon_Player(pDevice, pContext, strPrototypeTag)
{

}

CWeapon_Edgar::CWeapon_Edgar(const CWeapon_Edgar& rhs)
	: CWeapon_Player(rhs)
{
}

HRESULT CWeapon_Edgar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Edgar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	string path = "../Bin/DataFiles/Data_Weapon/Player/ELWinchester/Aim.json";
	json In_Json;
	CJson_Utility::Load_Json(path.c_str(), In_Json);
	m_pTransformCom->Load_FromJson(In_Json);

	return S_OK;
}

void CWeapon_Edgar::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Edgar::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Edgar::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Edgar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Edgar::Render_Shadow()
{
	FAILED_CHECK(__super::Render_Shadow());

	return S_OK;
}

void CWeapon_Edgar::Fire(_float3 vTargetPos, CCharacter* pTarget)
{
	__super::Fire(L"Prototype_GameObject_Bullet_Winchester", LAYER_PLAYER_BULLET, vTargetPos, pTarget);
}

HRESULT CWeapon_Edgar::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Model */
	//if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Weapon_Edgar"), NPC_WEAPON_EDGAR
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Player_Weapon_Winchester"), NPC_WEAPON_EDGAR
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

CWeapon_Edgar* CWeapon_Edgar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CWeapon_Edgar* pInstance = new CWeapon_Edgar(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Edgar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Edgar::Clone(void* pArg)
{
	CWeapon_Edgar* pInstance = new CWeapon_Edgar(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Edgar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Edgar::Pool()
{
	return new CWeapon_Edgar(*this);
}

void CWeapon_Edgar::Free()
{
	__super::Free();
}
