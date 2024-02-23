#include "..\Public\CollisionSimulationEventCallBack.h"
#include "PhysXCollider.h"
#include "PhysX_Manager.h"
#include "GameInstance.h"

CollisionSimulationEventCallBack::CollisionSimulationEventCallBack()
	: PxSimulationEventCallback()
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CollisionSimulationEventCallBack::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
	cout << "onConstraintBreak" << endl;
}

void CollisionSimulationEventCallBack::onWake(PxActor** actors, PxU32 count)
{
	cout << "onWake" << endl;
}

void CollisionSimulationEventCallBack::onSleep(PxActor** actors, PxU32 count)
{
	cout << "onSleep" << endl;
}

void CollisionSimulationEventCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	while (nbPairs--)
	{
		const PxContactPair& current = *pairs++;
	}
}

void CollisionSimulationEventCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	while (count--)
	{
		const PxTriggerPair& current = *pairs++;
		if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			CPhysXCollider* pColliderCom = m_pGameInstance->Find_PhysXCollider(*(_uint*)current.triggerActor->userData);
			CPhysXCollider* pOtherColliderCom = m_pGameInstance->Find_PhysXCollider(*(_uint*)current.otherActor->userData);

			if (pColliderCom->Get_ColliderDesc().bTrigger &&
				pOtherColliderCom->Get_ColliderDesc().bTrigger)
			{
				pColliderCom->PhysXCollisionEnter(pOtherColliderCom);
				pOtherColliderCom->PhysXCollisionEnter(pColliderCom);
			}

		}
		if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			CPhysXCollider* pColliderCom = m_pGameInstance->Find_PhysXCollider(*(_uint*)current.triggerActor->userData);
			CPhysXCollider* pOtherColliderCom = m_pGameInstance->Find_PhysXCollider(*(_uint*)current.otherActor->userData);
			if (pColliderCom->Get_ColliderDesc().bTrigger &&
				pOtherColliderCom->Get_ColliderDesc().bTrigger)
			{
				pColliderCom->PhysXCollisionExit(pOtherColliderCom);
				pOtherColliderCom->PhysXCollisionExit(pColliderCom);
			}
		}
		cout << "Shape is leaving trigger volume\n";
	}
}

void CollisionSimulationEventCallBack::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
	cout << "onAdvance" << endl;
}

void CollisionSimulationEventCallBack::Release()
{
	Safe_Release(m_pGameInstance);
	delete this;
}
