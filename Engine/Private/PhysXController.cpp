#include "..\Public\PhysXController.h"
#include "GameInstance.h"
#include "SMath.h"

CPhysXController::CPhysXController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CPhysXController::CPhysXController(const CPhysXController& rhs)
	: CComponent(rhs)
{
}

void CPhysXController::Set_EnableSimulation(const _bool In_EnableSimulation)
{
}

void CPhysXController::Set_CurrentCameraController()
{
}

PxController* CPhysXController::Get_Controller()
{
	return nullptr;
}

void CPhysXController::Enable_Gravity(const _bool In_bGravity)
{
	m_bEnableGravity = In_bGravity;
}

bool CPhysXController::filter(const PxController& a, const PxController& b)
{
	CPhysXController* pLeftControllerCom = m_pGameInstance->Find_PhysXController(*(_uint*)a.getUserData());
	CPhysXController* pRightControllerCom = m_pGameInstance->Find_PhysXController(*(_uint*)b.getUserData());

	if (nullptr == pLeftControllerCom || nullptr == pRightControllerCom)
		return false;

	if (pLeftControllerCom->Is_EnableSimulation() && pRightControllerCom->Is_EnableSimulation())
		return true;

	return false;
}

void CPhysXController::onShapeHit(const PxControllerShapeHit& hit)
{
}

void CPhysXController::onControllerHit(const PxControllersHit& hit)
{
	Callback_ControllerHit(hit);
}

void CPhysXController::onObstacleHit(const PxControllerObstacleHit& hit)
{
}

PxQueryHitType::Enum CPhysXController::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	if (false == Get_Enable())
		return PxQueryHitType::eNONE;

	if (!m_EnableColliderSimulation)
	{
		return PxQueryHitType::eNONE;
	}

	PxFilterData OtherFilter = shape->getSimulationFilterData();

	if ((m_FilterData.word0 & OtherFilter.word1) && (OtherFilter.word0 & m_FilterData.word1))
	{
		return PxQueryHitType::eBLOCK;
	}

	return PxQueryHitType::eNONE;
}

PxQueryHitType::Enum CPhysXController::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
	if (false == Get_Enable())
		return PxQueryHitType::eNONE;

	if (!m_EnableColliderSimulation)
	{
		return PxQueryHitType::eNONE;
	}

	PxFilterData OtherFilter = hit.shape->getSimulationFilterData();

	if ((filterData.word0 & OtherFilter.word1) && (OtherFilter.word0 & filterData.word1))
	{
		return PxQueryHitType::eBLOCK;
	}

	return PxQueryHitType::eNONE;
}

PxControllerCollisionFlags CPhysXController::Synchronize_Controller(CTransform* pTransform, PxF32 elapsedTime, PxControllerFilters& filters, _fvector In_vOffset)
{
	_vector vPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
	vPos += In_vOffset;
	vPos.m128_f32[3] = 1.f;

	m_pController->setFootPosition(SMath::Convert_PxExtendedVec3(pTransform->Get_Position()));

	Bind_FilterOptions(filters);

	return m_pController->move({ 0.f, 0.f, 0.f }, 0.f, elapsedTime, filters);
}

PxControllerCollisionFlags CPhysXController::Set_Position(_fvector In_vPosition, PxF32 elapsedTime, PxControllerFilters& filters)
{
	return PxControllerCollisionFlags();
}

PxControllerCollisionFlags CPhysXController::MoveWithRotation(_fvector disp, PxF32 minDist, PxF32 elapsedTime, PxControllerFilters& filters, const PxObstacleContext* obstacles, CTransform* pTransform, const _flag In_RootFlag)
{
	_vector vRotatedPosition = XMVector3TransformCoord(disp, SMath::Get_RotationMatrix(pTransform->Get_WorldMatrix()));

	Bind_FilterOptions(filters);

	PxVec3 vRotatedPositionFromPx = SMath::Convert_PxVec3(vRotatedPosition);

	PxExtendedVec3 PrePosition = m_pController->getPosition();


	if (DBL_EPSILON < vRotatedPositionFromPx.y || DBL_EPSILON < vRotatedPosition.m128_f32[1])
		int a = 0;

	if (!(In_RootFlag & (_byte)ROOTNODE_FLAG::X))
	{
		vRotatedPositionFromPx.x = 0.f;
	}

	if (!(In_RootFlag & (_byte)ROOTNODE_FLAG::Y))
	{
		vRotatedPositionFromPx.y = 0.f;
	}

	if (!(In_RootFlag & (_byte)ROOTNODE_FLAG::Z))
	{
		vRotatedPositionFromPx.z = 0.f;
	}

	auto Result = m_pController->move(vRotatedPositionFromPx, minDist, elapsedTime, filters, obstacles);

	PxExtendedVec3 CurPosition = m_pController->getPosition();
	PxVec3 RealMove = CurPosition - PrePosition;


	return Result;
}

void CPhysXController::Init_Controller(const PxCapsuleControllerDesc& In_ControllerDesc, const _uint In_CollisionLayer)
{
	m_pControllerDesc = In_ControllerDesc;
	m_pControllerDesc.reportCallback = this;
	m_pControllerDesc.userData = &m_iControllerIndex;
	m_FilterData.word0 = (1 << In_CollisionLayer);
	m_FilterData.word1 = CGameInstance::GetInstance()->Get_PhysXFilterGroup(In_CollisionLayer);
	Create_Controller();
}

void CPhysXController::Create_Controller()
{
	CGameInstance::GetInstance()->Create_Controller(m_pControllerDesc, &m_pController);
	m_pController->setUserData(&m_iControllerIndex);
}

void CPhysXController::Bind_FilterOptions(PxControllerFilters& Out_Filters)
{
	Out_Filters.mCCTFilterCallback = this;
	Out_Filters.mFilterCallback = this;
}

CPhysXController* CPhysXController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CPhysXController(pDevice, pContext);
}

CComponent* CPhysXController::Clone(void* pArg)
{
	return new CPhysXController(*this);
}

void CPhysXController::Free()
{
}


