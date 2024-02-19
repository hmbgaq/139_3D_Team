#include "stdafx.h"
#include "Player_Weapon_FlameBelcher.h"
#include "GameInstance.h"

CPlayer_Weapon_FlameBelcher::CPlayer_Weapon_FlameBelcher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CWeapon_Player(pDevice, pContext)
{

}

CPlayer_Weapon_FlameBelcher::CPlayer_Weapon_FlameBelcher(const CPlayer_Weapon_FlameBelcher& rhs)
	: CWeapon_Player(rhs)
{
}

HRESULT CPlayer_Weapon_FlameBelcher::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_FlameBelcher::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon_FlameBelcher::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer_Weapon_FlameBelcher::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPlayer_Weapon_FlameBelcher::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Weapon_FlameBelcher::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_FlameBelcher::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Player_Weapon_FlameBelcher"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

CPlayer_Weapon_FlameBelcher* CPlayer_Weapon_FlameBelcher::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Weapon_FlameBelcher* pInstance = new CPlayer_Weapon_FlameBelcher(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Weapon_FlameBelcher");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_FlameBelcher::Clone(void* pArg)
{
	CPlayer_Weapon_FlameBelcher* pInstance = new CPlayer_Weapon_FlameBelcher(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Weapon_FlameBelcher");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Weapon_FlameBelcher::Free()
{
	__super::Free();
}
