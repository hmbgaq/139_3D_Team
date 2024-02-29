#pragma once
#include "Component.h"
#include "Delegate.h"

BEGIN(Engine)

class CGameInstance;

class ENGINE_DLL CPhysXController :
	public CComponent, public PxControllerFilterCallback, public PxUserControllerHitReport, public PxQueryFilterCallback
{
protected:
	CPhysXController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXController(const CPhysXController& rhs);
	virtual ~CPhysXController() = default;

public:
	_uint	Get_PControllerIndex() const { return m_iControllerIndex; }
	_bool	Is_EnableSimulation() const { return m_EnableSimulation; }
	void	Set_EnableSimulation(const _bool In_EnableSimulation);
	void	Set_EnableColliderSimulation(const _bool In_EnableSimulation) { m_EnableColliderSimulation = In_EnableSimulation; }
	void	Set_CurrentCameraController();
	PxController* Get_Controller();

public:
	void	Enable_Gravity(const _bool In_bGravity);


public: // 피직스 컨트롤러 필터
	virtual bool filter(const PxController& a, const PxController& b) override;


public: // Hit Report
	virtual void onShapeHit(const PxControllerShapeHit& hit) override;
	virtual void onControllerHit(const PxControllersHit& hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit& hit) override;

	// PxQueryFilterCallback을(를) 통해 상속됨
	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override;
	virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit) override;


public:
	virtual PxControllerCollisionFlags	Synchronize_Controller(CTransform* pTransform, PxF32 elapsedTime, PxControllerFilters& filters, _fvector In_vOffset = { 0.f, 0.f, 0.f });
	virtual PxControllerCollisionFlags	Set_Position(_fvector In_vPosition, PxF32 elapsedTime, PxControllerFilters& filters);
	virtual PxControllerCollisionFlags	MoveWithRotation(_fvector disp, PxF32 minDist, PxF32 elapsedTime, PxControllerFilters& filters, const PxObstacleContext* obstacles, CTransform* pTransform, const _flag In_RootFlag = (_byte)ROOTNODE_FLAG::X + (_byte)ROOTNODE_FLAG::Y + (_byte)ROOTNODE_FLAG::Z);


public:
	virtual void	Init_Controller(const PxCapsuleControllerDesc& In_ControllerDesc, const _uint In_CollisionLayer);

protected:
	void			Bind_FilterOptions(PxControllerFilters& Out_Filters);

protected:
	void			Create_Controller();



public:
	FDelegate<const PxControllersHit&>	Callback_ControllerHit;


private:
	static	_uint							m_iClonedControllerIndex;
protected:
	_uint									m_iControllerIndex;

protected:
	PxCapsuleControllerDesc					m_pControllerDesc;
	PxController*							m_pController = { nullptr };

	_bool									m_EnableSimulation = true;
	_bool									m_EnableColliderSimulation = true;
	_bool									m_bEnableGravity = { true };


protected:
	PxFilterData							m_FilterData;

//protected:
//	CGameInstance*							m_pGameInstance = { nullptr };

public:
	static CPhysXController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	void Free();

};

END