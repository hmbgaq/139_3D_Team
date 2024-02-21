#include "stdafx.h"
#include "Player_Weapon_Shotgun.h"
#include "GameInstance.h"

CPlayer_Weapon_Shotgun::CPlayer_Weapon_Shotgun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CWeapon_Player(pDevice, pContext, strPrototypeTag)
{

}

CPlayer_Weapon_Shotgun::CPlayer_Weapon_Shotgun(const CPlayer_Weapon_Shotgun& rhs)
	: CWeapon_Player(rhs)
{
}

HRESULT CPlayer_Weapon_Shotgun::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_Shotgun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon_Shotgun::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer_Weapon_Shotgun::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPlayer_Weapon_Shotgun::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Weapon_Shotgun::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_Shotgun::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Player_Weapon_Shotgun"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

CPlayer_Weapon_Shotgun* CPlayer_Weapon_Shotgun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CPlayer_Weapon_Shotgun* pInstance = new CPlayer_Weapon_Shotgun(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Weapon_Shotgun");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_Shotgun::Clone(void* pArg)
{
	CPlayer_Weapon_Shotgun* pInstance = new CPlayer_Weapon_Shotgun(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Weapon_Shotgun");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_Shotgun::Pool()
{
	return new CPlayer_Weapon_Shotgun(*this);
}

void CPlayer_Weapon_Shotgun::Free()
{
	__super::Free();
}
