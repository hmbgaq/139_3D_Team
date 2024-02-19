#pragma once
#include "Base.h"

BEGIN(Engine)

class CPhysXCollider;
class CPhysXController;


class ENGINE_DLL CPhysXManager : public CBase
{
private:
	CPhysXManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPhysXManager() = default;

public:
	void	Register_PhysXCollider(CPhysXCollider* pPhysXCollider);
	CPhysXCollider*	Find_PhysXCollider(const _uint iPhysXColliderIndex);

	void	Register_PhysXController(CPhysXController* pPhysXController);
	CPhysXController*	Find_PhysXController(const _uint iPhysXControllerIndex);


public:
	HRESULT	Initialize(const _uint In_iNumLayer);
	void	Tick(_float fTimeDelta);


private:
	vector<_uint>			m_arrCheck;

private:
	// Foundation을 생성하는데 필요한 변수
	PxDefaultAllocator		m_Allocator;
	PxDefaultErrorCallback	m_ErrorCallback;

	PxFoundation* m_pFoundation;
	PxPhysics* m_pPhysics;
	PxScene* m_pScenes;
	PxMaterial* m_pMaterial;

	PxRigidDynamic* m_pTemp = nullptr;
	PxControllerManager* m_pControllerManager = nullptr;
	



	////Visual Debugger
	PxPvd* m_pPVD;

	PxCudaContextManager* m_pCudaContextManager = nullptr;
	// MeshCooking을 하기 위해 생성
	PxCooking* m_pCooking = nullptr;
	// CPU 리소스를 효율적으로 공유할 수 있도록 하기 위해 구현하는 것을 추천
	PxDefaultCpuDispatcher* m_pDispatcher = nullptr;

	

private:
	map<_uint, CPhysXCollider*>		m_pPhysXCollders;
	map<_uint, CPhysXController*>	m_pPhysXControllers;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


public:
	static CPhysXManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumLevels);
	virtual void Free() override;
};

END


