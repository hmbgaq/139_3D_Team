#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Cargo_Crane.h"
#include "Model.h"
#include "AttackObject.h"
#include "Tank.h"
#include "Data_Manager.h"

CCargo_Crane::CCargo_Crane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon(pDevice, pContext, strPrototypeTag)
{
}

CCargo_Crane::CCargo_Crane(const CCargo_Crane& rhs)
	: CWeapon(rhs)
{
}

HRESULT CCargo_Crane::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CCargo_Crane::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

HRESULT CCargo_Crane::Ready_Components()
{
	///* For.Com_Collider */
	m_iColliderSize = 1;
	m_pColliders.resize(m_iColliderSize);

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
	BoundingDesc.vExtents = _float3(2.0f, 2.0f, 2.0f);
	BoundingDesc.vCenter = _float3(0.f, -2.f, 0.f);

	FAILED_CHECK(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc));

	return S_OK;
}

void CCargo_Crane::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CCargo_Crane::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCargo_Crane::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCargo_Crane::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CCargo_Crane::OnCollisionEnter(CCollider* other)
{
	//CAttackObject* pTarget_AttackObject = Get_Target_AttackObject(other);

	//if (pTarget_AttackObject != nullptr)
	//{
	//	
	//}
}

void CCargo_Crane::OnCollisionStay(CCollider* other)
{
}

void CCargo_Crane::OnCollisionExit(CCollider* other)
{
}

#pragma region Create, Clone, Pool, Free

CCargo_Crane* CCargo_Crane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CCargo_Crane* pInstance = new CCargo_Crane(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCargo_Crane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCargo_Crane::Clone(void* pArg)
{
	CCargo_Crane* pInstance = new CCargo_Crane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCargo_Crane");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCargo_Crane::Pool()
{
	return new CCargo_Crane(*this);
}

void CCargo_Crane::Free()
{
	__super::Free();
}


#pragma endregion
