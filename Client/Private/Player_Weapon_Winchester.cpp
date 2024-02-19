#include "stdafx.h"
#include "Player_Weapon_Winchester.h"
#include "GameInstance.h"

CPlayer_Weapon_Winchester::CPlayer_Weapon_Winchester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CWeapon_Player(pDevice, pContext)
{

}

CPlayer_Weapon_Winchester::CPlayer_Weapon_Winchester(const CPlayer_Weapon_Winchester& rhs)
	: CWeapon_Player(rhs)
{
}

HRESULT CPlayer_Weapon_Winchester::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_Winchester::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon_Winchester::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer_Weapon_Winchester::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPlayer_Weapon_Winchester::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Weapon_Winchester::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_Winchester::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Player_Weapon_Winchester"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

CPlayer_Weapon_Winchester* CPlayer_Weapon_Winchester::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Weapon_Winchester* pInstance = new CPlayer_Weapon_Winchester(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Weapon_Winchester");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_Winchester::Clone(void* pArg)
{
	CPlayer_Weapon_Winchester* pInstance = new CPlayer_Weapon_Winchester(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Weapon_Winchester");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Weapon_Winchester::Free()
{
	__super::Free();
}
