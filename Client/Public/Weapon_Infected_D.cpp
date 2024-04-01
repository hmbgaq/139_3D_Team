#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Weapon_Infected_D.h"
#include "Effect_Manager.h"
#include "Effect_Trail.h"

CWeapon_Infected_D::CWeapon_Infected_D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon_Infected(pDevice, pContext, strPrototypeTag)
{
}

CWeapon_Infected_D::CWeapon_Infected_D(const CWeapon_Infected_D& rhs)
	: CWeapon_Infected(rhs)
{
}

HRESULT CWeapon_Infected_D::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_Infected_D::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CWeapon_Infected_D::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Infected_D::Tick(_float fTimeDelta)
{
	if (m_bBombActive)
	{
		m_fTimeAcc += fTimeDelta;

		if (m_fTimeAcc > 1.f)
		{
			m_fTimeAcc = 0.f;
			m_bBombActive = false;
			m_pColliders[0]->Set_Enable(true);
			m_pColliders[0]->Update(m_WorldMatrix);
		}
	}
	__super::Tick(fTimeDelta);

}

void CWeapon_Infected_D::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//! 유정: 트레일 테스트
	if (nullptr != m_pTrail)
	{
		m_pTrail->Tick_Trail(fTimeDelta, m_WorldMatrix);
	}
}

HRESULT CWeapon_Infected_D::Render()
{
	return S_OK;
}

HRESULT CWeapon_Infected_D::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Collider */
	{
		m_iColliderSize = 1;
		m_pColliders.resize(m_iColliderSize);

		CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
		BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
		BoundingDesc.fRadius = { 3.f };
		BoundingDesc.vCenter = _float3(0.f, -1.f * (BoundingDesc.fRadius / 2.f), 0.f);

		FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc));
		m_pColliders[0]->Set_Enable(false);
	}

	//! 유정: 트레일 테스트 - 이 트레일 아님 
	//m_pTrail = EFFECT_MANAGER->Ready_Trail(iNextLevel, LAYER_EFFECT, "Test_Trail.json", this);

	return S_OK;
}

HRESULT CWeapon_Infected_D::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	return S_OK;
}

#pragma region Create, Clone, Pool, Free

CWeapon_Infected_D* CWeapon_Infected_D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CWeapon_Infected_D* pInstance = new CWeapon_Infected_D(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Infected_D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Infected_D::Clone(void* pArg)
{
	CWeapon_Infected_D* pInstance = new CWeapon_Infected_D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Infected_D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Infected_D::Pool()
{
	return new CWeapon_Infected_D(*this);
}

void CWeapon_Infected_D::Free()
{
	__super::Free();
}


#pragma endregion
