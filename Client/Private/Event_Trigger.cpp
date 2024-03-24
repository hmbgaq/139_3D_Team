#include "..\Public\Event_Trigger.h"
#include "Player.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Transform.h"

CEvent_Trigger::CEvent_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CEvent_Trigger::Initialize(void* pArg)
{
	m_tTriggerDesc = *(TRIGGER_DESC*)pArg;

	if (FAILED(Ready_Component()))
		return E_FAIL;
	
	return S_OK;
}

void CEvent_Trigger::Activate()
{
}

_bool CEvent_Trigger::Activate_Condition()
{
	if (nullptr == m_pActor || nullptr == m_pColliderCom)
	{
		if (m_bSetFindActor == false)
		{
			m_pActor = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player());

			if(m_pActor == nullptr)
				return false;
			else
			{
				m_bSetFindActor = true;
				Safe_AddRef(m_pActor);
			}
		}

		return false;
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if(m_bSetFindActor == true && true == m_pColliderCom->Is_Collision(m_pActor->Get_Collider()))
		return true;
	
	

	return false;
}

_bool CEvent_Trigger::End_Condition()
{
	return false;
}

_bool CEvent_Trigger::Write_Json(json& Out_Json)
{
	m_pTransformCom->Write_Json(Out_Json["Component"]);

	return false;
}

void CEvent_Trigger::Load_FromJson(const json& In_Json)
{
	
	_float4x4 WorldMatrix;

	ZeroMemory(&WorldMatrix, sizeof(_float4x4));
	CJson_Utility::Load_JsonFloat4x4(In_Json["Component"]["Transform"], WorldMatrix);

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

}

HRESULT CEvent_Trigger::Ready_Component()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();
	
	/* For.Com_Collider */
	{
		CBounding_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};
		BoundingDesc.iLayer = ECast(COLLISION_LAYER::NONE);
		BoundingDesc.vExtents = _float3(0.22f, 0.8f, 0.22f);
		BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	
		CComponent* pComponent = m_pGameInstance->Clone_Component(iNextLevel, TEXT("Prototype_Component_Collider_AABB"), &BoundingDesc);
		
		if(pComponent == nullptr)
			return E_FAIL;

		m_pColliderCom = dynamic_cast<CCollider*>(pComponent);

		if(m_pColliderCom == nullptr)
			return E_FAIL;

		m_pTransformCom = CTransform::Create(m_pDevice, m_pContext, 0.f, 0.f);
		NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);

		
	}

	return S_OK;
}


void CEvent_Trigger::Set_ColliderSize(_float3 vColliderSize)
{
	CBounding* pBounding = m_pColliderCom->Get_Bounding();

	CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

	if (pAABB == nullptr)
		return;

	BoundingBox* pBox = pAABB->Get_OriginBounding();

	pBox->Extents = vColliderSize;
	m_tTriggerDesc.vColliderSize = vColliderSize;
}

void CEvent_Trigger::Set_ColliderCenter(_float3 vColliderCenter)
{
	CBounding* pBounding = m_pColliderCom->Get_Bounding();

	CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

	if (pAABB == nullptr)
		return;

	BoundingBox* pBox = pAABB->Get_OriginBounding();

	pBox->Center = vColliderCenter;
	m_tTriggerDesc.vColliderCenter = vColliderCenter;
}

void CEvent_Trigger::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	if (m_pActor != nullptr)
	{
		Safe_Release(m_pActor);
	}

	if(nullptr != m_pColliderCom)
		Safe_Release(m_pColliderCom);

	if(nullptr != m_pTransformCom)
		Safe_Release(m_pTransformCom);


}
