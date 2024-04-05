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
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::OBSTACLE);
	BoundingDesc.vExtents = _float3(2.0f, 2.0f, 2.0f);
	BoundingDesc.vCenter = _float3(-2.f, 0.f, 0.f);

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

void CCargo_Crane::Push(CCollider* other)
{
	if (nullptr == other || false == other->Get_Enable()) return;

	if (other->Get_Layer() == ECast(COLLISION_LAYER::MONSTER)
		|| other->Get_Layer() == ECast(COLLISION_LAYER::PLAYER))
	{
		CGameObject* pOwner = other->Get_Owner();
		if (nullptr == pOwner) return;

		//Get_WorldPosition();
		CCharacter* pParentCharacter = dynamic_cast<CCharacter*>(pOwner->Get_Object_Owner());
		if (pParentCharacter)
		{
			_float3 vPos = Get_WorldPosition();
			_vector vDir = pParentCharacter->Calc_Look_Dir_XZ(XMLoadFloat3(&vPos));
			pParentCharacter->Add_Force(vDir, 17.f * m_pGameInstance->Get_TimeDelta());
		}
	}
	else
	{
		CAttackObject* pAttackObject = Get_Target_AttackObject(other);
		if (pAttackObject) 
		{
			pAttackObject->Set_Dead(true);
		}
	}
}

void CCargo_Crane::OnCollisionEnter(CCollider* other)
{
	Push(other);
}

void CCargo_Crane::OnCollisionStay(CCollider* other)
{
	Push(other);
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
