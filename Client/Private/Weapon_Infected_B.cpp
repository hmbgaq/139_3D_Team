#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Weapon_Infected_B.h"

CWeapon_Infected_B::CWeapon_Infected_B(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon_Infected(pDevice, pContext, strPrototypeTag)
{
}

CWeapon_Infected_B::CWeapon_Infected_B(const CWeapon_Infected_B& rhs)
	: CWeapon_Infected(rhs)
{
}

HRESULT CWeapon_Infected_B::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_Infected_B::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CWeapon_Infected_B::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Infected_B::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWeapon_Infected_B::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Infected_B::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CWeapon_Infected_B::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Collider */
	{
		m_iColliderSize = 1;
		m_pColliders.resize(m_iColliderSize);

		CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
		BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
		BoundingDesc.fRadius = { 0.15f };
		BoundingDesc.vCenter = _float3(0.f, -0.1f, 0.f);

		FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc));
	}

	return S_OK;
}

HRESULT CWeapon_Infected_B::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	return S_OK;
}

#pragma region Create, Clone, Pool, Free

CWeapon_Infected_B* CWeapon_Infected_B::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CWeapon_Infected_B* pInstance = new CWeapon_Infected_B(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Infected_B");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Infected_B::Clone(void* pArg)
{
	CWeapon_Infected_B* pInstance = new CWeapon_Infected_B(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Infected_B");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Infected_B::Pool()
{
	return new CWeapon_Infected_B(*this);
}

void CWeapon_Infected_B::Free()
{
	__super::Free();
}


#pragma endregion
