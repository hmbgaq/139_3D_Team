#pragma once
#include "Base.h"

BEGIN(Engine)

class CPhysXCollider;
class CPhysXController;
class CGameInstance;


class ENGINE_DLL CPhysX_Manager : public CBase
{
private:
	CPhysX_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPhysX_Manager() = default;

//PxSimulationEventCallback* Get_CollisionSimulationEventCallBack()
//{
//	return m_pCollisionSimulationEventCallBack;
//}

public:
	HRESULT	Initialize(const _uint In_iNumLayer);
	void	Tick(_float fTimeDelta);

private:
	void	Garbage_Collector();


public:
	void				Register_PhysXCollider(CPhysXCollider* pPhysXCollider);
	CPhysXCollider*		Find_PhysXCollider(const _uint iPhysXColliderIndex);

	void				Register_PhysXController(CPhysXController* pPhysXController);
	CPhysXController*	Find_PhysXController(const _uint iPhysXControllerIndex);

	void				Check_PhysXFilterGroup(const _uint In_iLeftLayer, const _uint In_iRightLayer);
	_uint				Get_PhysXFilterGroup(const _uint In_iIndex);

	HRESULT				Create_Scene(PxVec3 Gravity = PxVec3(0.0f, -9.81f, 0.0f));
	HRESULT				Delete_Scene();

	PxRigidDynamic*		Create_DynamicActor(const PxTransform& transform, const PxGeometry& geometry, PxMaterial* pMaterial = nullptr);
	PxRigidDynamic*		Create_DynamicActor(const PxTransform& transform);
	PxRigidStatic*		Create_StaticActor(const PxTransform& transform, const PxGeometry& geometry, PxMaterial* pMaterial = nullptr);
	PxRigidStatic*		Create_StaticActor(const PxTransform& transform);

	void				Add_DynamicActorAtCurrentScene(PxRigidDynamic& DynamicActor);
	void				Add_StaticActorAtCurrentScene(PxRigidStatic& StaticActor);


	void				Create_Material(_float fStaticFriction, _float fDynamicFriction, _float fRestitution, PxMaterial** ppOut);
	void				Create_ConvexMesh(PxVec3** pVertices, _uint iNumVertice, PxConvexMesh** ppOut);
	void				Create_ConvexMesh(const PxConvexMeshDesc& In_MeshDesc, PxConvexMesh** ppOut);
	void				Create_Shape(const PxGeometry& Geometry, PxMaterial* pMaterial, const _bool isExculsive, const PxShapeFlags In_ShapeFlags, PxShape** ppOut);
	void				Create_MeshFromTriangles(const PxTriangleMeshDesc& In_MeshDesc, PxTriangleMesh** ppOut);
	void				Create_Controller(const PxCapsuleControllerDesc& In_ControllerDesc, PxController** ppOut);



private:
	vector<_uint>			m_arrCheck;
	_float					m_fTimeAcc = { 0.f };

private:
	// Foundation을 생성하는데 필요한 변수
	physx::PxDefaultAllocator		m_Allocator;
	physx::PxDefaultErrorCallback	m_ErrorCallback;

	physx::PxFoundation* m_pFoundation = { nullptr };
	physx::PxPhysics* m_pPhysics = { nullptr };
	physx::PxScene* m_pScene = { nullptr };
	physx::PxMaterial* m_pMaterial = { nullptr };

	physx::PxRigidDynamic* m_pTemp = { nullptr };
	physx::PxControllerManager* m_pControllerManager = nullptr;
	
	////Visual Debugger
	physx::PxPvd* m_pPVD = { nullptr };

	physx::PxCooking* m_pCooking = { nullptr };
	physx::PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
	//PxCudaContextManager* m_pCudaContextManager = nullptr;\


private:
	map<_uint, CPhysXCollider*>		m_pPhysXCollders;
	map<_uint, CPhysXController*>	m_pPhysXControllers;

private:
	class CollisionSimulationEventCallBack* m_pCollisionSimulationEventCallBack = { nullptr };



private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };


public:
	static CPhysX_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint In_iNumLayer);
	virtual void Free() override;
};

END

