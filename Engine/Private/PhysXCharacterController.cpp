#include "PhysXCharacterController.h"
#include "Transform.h"
#include "PhysXCollider.h"
#include "GameInstance.h"
#include "SMath.h"


CPhysXCharacterController::CPhysXCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPhysXController(pDevice, pContext)
{
}

CPhysXCharacterController::CPhysXCharacterController(const CPhysXCharacterController& rhs)
	: CPhysXController(rhs)
{
}

HRESULT CPhysXCharacterController::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CPhysXCharacterController::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

bool CPhysXCharacterController::filter(const PxController& a, const PxController& b)
{
	return __super::filter(a, b);
}

void CPhysXCharacterController::onShapeHit(const PxControllerShapeHit& hit)
{
	m_pLastCollisionActor = hit.actor;
	m_pLastCollisionShape = hit.shape;
}

void CPhysXCharacterController::onControllerHit(const PxControllersHit& hit)
{
	__super::onControllerHit(hit);

}

void CPhysXCharacterController::onObstacleHit(const PxControllerObstacleHit& hit)
{
}

PxControllerBehaviorFlags CPhysXCharacterController::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags CPhysXCharacterController::getBehaviorFlags(const PxController& controller)
{
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags CPhysXCharacterController::getBehaviorFlags(const PxObstacle& obstacle)
{
	return PxControllerBehaviorFlags(0);
}


void CPhysXCharacterController::Init_Controller(const PxCapsuleControllerDesc& In_ControllerDesc, const _uint In_CollisionLayer)
{
	m_pControllerDesc = In_ControllerDesc;
	m_pControllerDesc.reportCallback = this;
	m_pControllerDesc.behaviorCallback = this;
	m_vPrePosition = { (_float)m_pControllerDesc.position.x, (_float)m_pControllerDesc.position.y, (_float)m_pControllerDesc.position.z };
	m_FilterData.word0 = (1 << In_CollisionLayer);
	m_FilterData.word1 = m_pGameInstance->Get_PhysXFilterGroup(In_CollisionLayer);
	Create_Controller();
}

//void CPhysXCharacterController::Synchronize_Transform(CTransform* pTransform, _fvector In_vOffset)
//{
//	if (!m_pController)
//		return;
//
//	__super::Synchronize_Transform(pTransform, In_vOffset);
//}

PxControllerCollisionFlags CPhysXCharacterController::Synchronize_Controller(CTransform* pTransform, PxF32 elapsedTime, PxControllerFilters& filters, _fvector In_vOffset)
{
	filters.mFilterCallback = this;

	return __super::Synchronize_Controller(pTransform, elapsedTime, filters, In_vOffset);
}

PxControllerCollisionFlags CPhysXCharacterController::Set_Position(_fvector In_vPosition, PxF32 elapsedTime, PxControllerFilters& filters)
{
	filters.mFilterCallback = this;

	return  __super::Set_Position(In_vPosition, elapsedTime, filters);
}

PxControllerCollisionFlags CPhysXCharacterController::MoveWithRotation(_fvector disp, PxF32 minDist, PxF32 elapsedTime, PxControllerFilters& filters, const PxObstacleContext* obstacles, weak_ptr<CTransform> pTransform, const _flag In_RootFlag)
{
	filters.mFilterCallback = this;

	return __super::MoveWithRotation(disp, minDist, elapsedTime, filters, obstacles, pTransform);
}

//PxControllerCollisionFlags CPhysXCharacterController::Move(_fvector disp, PxF32 minDist, PxF32 elapsedTime, PxControllerFilters& filters, const PxObstacleContext* obstacles)
//{
//	filters.mFilterCallback = this;
//
//	return  __super::Move(disp, minDist, elapsedTime, filters, obstacles);
//}
//
//PxControllerCollisionFlags CPhysXCharacterController::MoveGravity(const _float fDeltaTime, PxControllerFilters& filters)
//{
//	if (!Get_Enable())
//		return PxControllerCollisionFlags();
//
//	if (!m_bEnableGravity)
//		return PxControllerCollisionFlags();
//
//	filters.mFilterCallback = this;
//
//	PxControllerCollisionFlags ResultFlags = __super::MoveGravity(fDeltaTime, filters);
//
//	return ResultFlags;
//}

_bool CPhysXCharacterController::Move_FootOffset(const _float fTimeDelta)
{
	if (!m_pLastCollisionActor || !m_pLastCollisionShape)
	{
		cout << "No Actor or Shape." << endl;
		return false;
	}

	PxRaycastHit newHit;

	PxVec3 RayPos = SMath::Convert_PxVec3(m_pController->getPosition());
	PxVec3 RayDir = -m_pController->getUpDirection();

	//m_pLastCollisionActor.

	PxU32 iHitCount = PxShapeExt::raycast(
		*m_pLastCollisionShape, *m_pLastCollisionActor
		, RayPos, RayDir,
		m_pController->getStepOffset() * 100.f,
		PxHitFlag::ePOSITION, 1, &newHit);

	if (iHitCount != 0)
	{
		m_pController->setFootPosition(SMath::Convert_PxExtendedVec3(newHit.position));
		return true;
	}

	return false;
}



CPhysXCharacterController* CPhysXCharacterController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	CPhysXCharacterController* m_pInstance = new CPhysXCharacterController(pDevice, pContext);
	return m_pInstance;

}

CComponent* CPhysXCharacterController::Clone(void* pArg)
{
	return new CPhysXCharacterController(*this);
}

void CPhysXCharacterController::Free()
{
}

