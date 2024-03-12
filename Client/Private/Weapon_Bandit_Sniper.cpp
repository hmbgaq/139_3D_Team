#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Weapon_Bandit_Sniper.h"

CWeapon_Bandit_Sniper::CWeapon_Bandit_Sniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon(pDevice, pContext, strPrototypeTag)
{
}

CWeapon_Bandit_Sniper::CWeapon_Bandit_Sniper(const CWeapon_Bandit_Sniper& rhs)
	: CWeapon(rhs)
{
}

HRESULT CWeapon_Bandit_Sniper::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_Bandit_Sniper::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CWeapon_Bandit_Sniper::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Bandit_Sniper::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Bandit_Sniper::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Bandit_Sniper::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CWeapon_Bandit_Sniper::Ready_Components()
{
	/* 스나이퍼 무기랑 충돌을 해야하는가 ? 딱히 뭔가 없던거 같던데 */
	//_uint iNextLevel = m_pGameInstance->Get_NextLevel();
	//
	///* For.Com_Collider */
	//{
	//	m_iColliderSize = 1;
	//	m_pColliders.resize(m_iColliderSize);
	//
	//	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	//	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
	//	BoundingDesc.fRadius = { 0.15f };
	//	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
	//
	//	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc));
	//}

	return S_OK;
}

HRESULT CWeapon_Bandit_Sniper::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	return S_OK;
}


#pragma region Create, Clone, Pool, Free

CWeapon_Bandit_Sniper* CWeapon_Bandit_Sniper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CWeapon_Bandit_Sniper* pInstance = new CWeapon_Bandit_Sniper(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Bandit_Sniper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Bandit_Sniper::Clone(void* pArg)
{
	CWeapon_Bandit_Sniper* pInstance = new CWeapon_Bandit_Sniper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Bandit_Sniper");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Bandit_Sniper::Pool()
{
	return new CWeapon_Bandit_Sniper(*this);
}

void CWeapon_Bandit_Sniper::Free()
{
	__super::Free();
}


#pragma endregion
