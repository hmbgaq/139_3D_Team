#include "stdafx.h"
#include "Player_Weapon_CripplingRod.h"
#include "GameInstance.h"
#include "MasterCamera.h"
#include "SpringCamera.h"
#include "Data_Manager.h"

CPlayer_Weapon_CripplingRod::CPlayer_Weapon_CripplingRod(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CWeapon_Player(pDevice, pContext, strPrototypeTag)
{

}

CPlayer_Weapon_CripplingRod::CPlayer_Weapon_CripplingRod(const CPlayer_Weapon_CripplingRod& rhs)
	: CWeapon_Player(rhs)
{
}

HRESULT CPlayer_Weapon_CripplingRod::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_CripplingRod::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	string path = "../Bin/DataFiles/Data_Weapon/Player/ELWinchester/Aim.json";
	json In_Json;
	CJson_Utility::Load_Json(path.c_str(), In_Json);
	m_pTransformCom->Load_FromJson(In_Json);

	return S_OK;
}

void CPlayer_Weapon_CripplingRod::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer_Weapon_CripplingRod::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPlayer_Weapon_CripplingRod::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Weapon_CripplingRod::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon_CripplingRod::Fire(_float3 vTargetPos, CCharacter* pTarget)
{
	__super::Fire(L"Prototype_GameObject_Bullet_Winchester", LAYER_PLAYER_BULLET, vTargetPos, pTarget);
}


HRESULT CPlayer_Weapon_CripplingRod::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Player_Weapon_CripplingRod"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

CPlayer_Weapon_CripplingRod* CPlayer_Weapon_CripplingRod::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CPlayer_Weapon_CripplingRod* pInstance = new CPlayer_Weapon_CripplingRod(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Weapon_CripplingRod");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_CripplingRod::Clone(void* pArg)
{
	CPlayer_Weapon_CripplingRod* pInstance = new CPlayer_Weapon_CripplingRod(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Weapon_CripplingRod");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_CripplingRod::Pool()
{
	return new CPlayer_Weapon_CripplingRod(*this);
}

void CPlayer_Weapon_CripplingRod::Free()
{
	__super::Free();
}
