#pragma once
#include "PhysXController.h"

BEGIN(Engine)

class CTransform;

class ENGINE_DLL CPhysXCharacterController :
	public CPhysXController, public PxControllerBehaviorCallback
{
protected:
	CPhysXCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXCharacterController(const CPhysXCharacterController& rhs);
	virtual ~CPhysXCharacterController() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;
	//virtual void	Start() override;

public: // 피직스 컨트롤러 필터
	virtual bool filter(const PxController& a, const PxController& b) override;

public: // Hit Report
	virtual void onShapeHit(const PxControllerShapeHit& hit) override;
	virtual void onControllerHit(const PxControllersHit& hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit& hit) override;

	// PxControllerBehaviorCallback을(를) 통해 상속됨
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle) override;




public:
	virtual void Init_Controller(const PxCapsuleControllerDesc& In_ControllerDesc, const _uint In_CollisionLayer) override;

public:
	//virtual void						Synchronize_Transform(CTransform* pTransform, _fvector In_vOffset = { 0.f, 0.f, 0.f }) override;
	virtual PxControllerCollisionFlags	Synchronize_Controller(CTransform* pTransform, PxF32 elapsedTime, PxControllerFilters& filters, _fvector In_vOffset = { 0.f, 0.f, 0.f }) override;
	virtual PxControllerCollisionFlags	Set_Position(_fvector In_vPosition, PxF32 elapsedTime, PxControllerFilters& filters) override;
	virtual PxControllerCollisionFlags	MoveWithRotation(_fvector disp, PxF32 minDist, PxF32 elapsedTime, PxControllerFilters& filters, const PxObstacleContext* obstacles, CTransform* pTransform, const _flag In_RootFlag = (_byte)ROOTNODE_FLAG::X + (_byte)ROOTNODE_FLAG::Y + (_byte)ROOTNODE_FLAG::Z) override;
	//virtual PxControllerCollisionFlags	Move(_fvector disp, PxF32 minDist, PxF32 elapsedTime, PxControllerFilters& filters, const PxObstacleContext* obstacles = nullptr) override;
	//virtual PxControllerCollisionFlags	MoveGravity(const _float fDeltaTime, PxControllerFilters& filters)  override;

public:

public:
	_bool Move_FootOffset(const _float fTimeDelta);

private:
	_float3 m_vPrePosition;
	PxShape* m_pLastCollisionShape = nullptr;
	PxRigidActor* m_pLastCollisionActor = nullptr;
	PxControllerCollisionFlags m_PreResultFlags;


public:
	static CPhysXCharacterController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	void Free();

};

END