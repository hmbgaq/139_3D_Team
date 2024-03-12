#include "stdafx.h"
#include "GameInstance.h"
#include "Weapon_Infected.h"
#include "Bone.h"

CWeapon_Infected::CWeapon_Infected(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon(pDevice, pContext, strPrototypeTag)
{
}

CWeapon_Infected::CWeapon_Infected(const CWeapon_Infected& rhs)
	: CWeapon(rhs)
{
}

HRESULT CWeapon_Infected::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_Infected::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CWeapon_Infected::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Infected::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Infected::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Infected::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CWeapon_Infected::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	return S_OK;
}

HRESULT CWeapon_Infected::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	return S_OK;
}

void CWeapon_Infected::Free()
{
	__super::Free();
}

