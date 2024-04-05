#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "HitPoint_Crane.h"
#include "Model.h"
#include "AttackObject.h"
#include "Tank.h"
#include "Data_Manager.h"
#include "Crane.h"

CHitPoint_Crane::CHitPoint_Crane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon(pDevice, pContext, strPrototypeTag)
{
}

CHitPoint_Crane::CHitPoint_Crane(const CHitPoint_Crane& rhs)
	: CWeapon(rhs)
{
}

HRESULT CHitPoint_Crane::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CHitPoint_Crane::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	

	return S_OK;
}

HRESULT CHitPoint_Crane::Ready_Components()
{
	///* For.Com_Collider */
	m_iColliderSize = 1;
	m_pColliders.resize(m_iColliderSize);

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::OBSTACLE);
	BoundingDesc.vExtents = _float3(2.0f, 2.0f, 2.0f);
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	FAILED_CHECK(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc));

	return S_OK;
}

void CHitPoint_Crane::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CHitPoint_Crane::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CHitPoint_Crane::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CHitPoint_Crane::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CHitPoint_Crane::OnCollisionEnter(CCollider* other)
{
	CAttackObject* pTarget_AttackObject = Get_Target_AttackObject(other);

	if (pTarget_AttackObject != nullptr)
	{
		CCrane* pCrane = dynamic_cast<CCrane*>(Get_PartOwner());
		if (pCrane) 
		{
			pCrane->Activate();
		}
	}
}

void CHitPoint_Crane::OnCollisionStay(CCollider* other)
{
}

void CHitPoint_Crane::OnCollisionExit(CCollider* other)
{
}

#pragma region Create, Clone, Pool, Free

CHitPoint_Crane* CHitPoint_Crane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CHitPoint_Crane* pInstance = new CHitPoint_Crane(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHitPoint_Crane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHitPoint_Crane::Clone(void* pArg)
{
	CHitPoint_Crane* pInstance = new CHitPoint_Crane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHitPoint_Crane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHitPoint_Crane::Pool()
{
	return new CHitPoint_Crane(*this);
}

void CHitPoint_Crane::Free()
{
	__super::Free();
}


#pragma endregion
