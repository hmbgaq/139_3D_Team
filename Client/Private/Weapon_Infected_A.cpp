#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Weapon_Infected_A.h"

#include "Effect_Manager.h"
#include "Effect_Trail.h"


CWeapon_Infected_A::CWeapon_Infected_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon_Infected(pDevice, pContext, strPrototypeTag)
{
}

CWeapon_Infected_A::CWeapon_Infected_A(const CWeapon_Infected_A& rhs)
	: CWeapon_Infected(rhs)
{
}

HRESULT CWeapon_Infected_A::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_Infected_A::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CWeapon_Infected_A::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Infected_A::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Infected_A::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	if (nullptr != m_pTrail)
	{
		//! 유정: 트레일 테스트
		m_pTrail->Tick_Trail(fTimeDelta, m_WorldMatrix);
	}

}

HRESULT CWeapon_Infected_A::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}


HRESULT CWeapon_Infected_A::Ready_Components()
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
	m_pTrail = EFFECT_MANAGER->Ready_Trail(m_pGameInstance->Get_NextLevel(), LAYER_EFFECT, "Test_Trail.json");
	m_pTrail->Set_Play(false);		// 시작은 끄기

	return S_OK;
}

HRESULT CWeapon_Infected_A::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	return S_OK;
}

#pragma region Create, Clone, Pool, Free

CWeapon_Infected_A* CWeapon_Infected_A::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CWeapon_Infected_A* pInstance = new CWeapon_Infected_A(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Infected_A");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Infected_A::Clone(void* pArg)
{
	CWeapon_Infected_A* pInstance = new CWeapon_Infected_A(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Infected_A");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Infected_A::Pool()
{
	return new CWeapon_Infected_A(*this);
}

void CWeapon_Infected_A::Free()
{
	__super::Free();

}


#pragma endregion
