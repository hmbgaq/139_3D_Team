#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Weapon_Infected_A.h"

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
		CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
		BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
		BoundingDesc.fRadius = { 0.1f };
		BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

		FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc));
	}

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
