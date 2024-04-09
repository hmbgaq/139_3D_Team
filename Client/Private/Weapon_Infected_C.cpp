#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Weapon_Infected_C.h"

#include "Effect_Manager.h"
#include "Effect_Trail.h"

CWeapon_Infected_C::CWeapon_Infected_C(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon_Infected(pDevice, pContext, strPrototypeTag)
{
}

CWeapon_Infected_C::CWeapon_Infected_C(const CWeapon_Infected_C& rhs)
	: CWeapon_Infected(rhs)
{
}

HRESULT CWeapon_Infected_C::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_Infected_C::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CWeapon_Infected_C::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Infected_C::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Infected_C::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//! 유정: 트레일 테스트
	if (nullptr != m_pTrail)
	{
		m_pTrail->Tick_Trail(fTimeDelta, m_WorldMatrix);
	}
}

HRESULT CWeapon_Infected_C::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CWeapon_Infected_C::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Collider */
	{
		m_iColliderSize = 1;
		m_pColliders.resize(m_iColliderSize);

		CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
		BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
		BoundingDesc.fRadius = { 0.2f };
		BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

		FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc));
	}

	//! 유정: 트레일 테스트
	m_pTrail = EFFECT_MANAGER->Ready_Trail(iNextLevel, LAYER_EFFECT, "Test_Trail.json");
	m_pTrail->Set_Play(false);		// 시작은 끄기

	return S_OK;
}

HRESULT CWeapon_Infected_C::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	return S_OK;
}

#pragma region Create, Clone, Pool, Free

CWeapon_Infected_C* CWeapon_Infected_C::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CWeapon_Infected_C* pInstance = new CWeapon_Infected_C(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Infected_C");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Infected_C::Clone(void* pArg)
{
	CWeapon_Infected_C* pInstance = new CWeapon_Infected_C(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Infected_C");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Infected_C::Pool()
{
	return new CWeapon_Infected_C(*this);
}

void CWeapon_Infected_C::Free()
{
	__super::Free();

}


#pragma endregion
