#pragma once
#include "Base.h"

BEGIN(Engine)

class CPhysXCollider;
class CPhysXController;


class ENGINE_DLL CPhysX_Manager : public CBase
{
private:
	CPhysX_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPhysX_Manager() = default;



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

	PxRigidDynamic*		Create_DynamicActor(const PxTransform& transform, const PxGeometry& geometry, PxMaterial* pMaterial = nullptr);
	PxRigidDynamic*		Create_DynamicActor(const PxTransform& transform);
	PxRigidStatic*		Create_StaticActor(const PxTransform& transform, const PxGeometry& geometry, PxMaterial* pMaterial = nullptr);
	PxRigidStatic*		Create_StaticActor(const PxTransform& transform);

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
	PxDefaultAllocator		m_Allocator;
	PxDefaultErrorCallback	m_ErrorCallback;

	PxFoundation* m_pFoundation = { nullptr };
	PxPhysics* m_pPhysics = { nullptr };
	PxScene* m_pScene = { nullptr };
	PxMaterial* m_pMaterial = { nullptr };

	PxRigidDynamic* m_pTemp = { nullptr };
	PxControllerManager* m_pControllerManager = nullptr;
	
	////Visual Debugger
	PxPvd* m_pPVD = { nullptr };

	PxCooking* m_pCooking = { nullptr };
	//PxDefaultCpuDispatcher* m_pDispatcher = nullptr;


private:
	map<_uint, CPhysXCollider*>		m_pPhysXCollders;
	map<_uint, CPhysXController*>	m_pPhysXControllers;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


public:
	static CPhysX_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint In_iNumLayer);
	virtual void Free() override;
};

END


