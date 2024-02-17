#pragma once
#include "Base.h"

BEGIN(Engine)


class ENGINE_DLL CPhysXManager : public CBase
{
private:
	CPhysXManager();
	virtual ~CPhysXManager() = default;

public:
	HRESULT Initialize();

private:
	//PxDefaultAllocator					m_PxAllocator;
	//PxDefaultErrorCallback				m_PXErrorCallback;
	//PxDefaultCpuDispatcher*				m_pPxDispatcher = nullptr;
	//PxTolerancesScale					m_PxTolerancesScale;

	//PxFoundation*						m_pPxFoundation = { nullptr };
	//PxPhysics*							m_pPhysX = { nullptr };

	//PxScene*							m_PxScene = { nullptr };
	//PxMaterial*							m_pMaterial = { nullptr };

	PxFoundation* m_PxFoundation = nullptr;

	PxDefaultAllocator					m_PxAllocator;
	PxDefaultErrorCallback				m_PXErrorCallback;
	PxPhysics* m_PhysX = nullptr;


	PxPvd* m_Pvd = nullptr;
	std::string							m_pvdIPAddress = "127.0.0.1";
	int									m_pvdPortNumber = 5425;
	_uint								m_pvdTimeOutSeconds = 10;

	PxDefaultCpuDispatcher* m_PxDispatcher = nullptr;
	PxScene* m_PxScene = nullptr;


public:
	static CPhysXManager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END


//PxPvd* m_Pvd = nullptr;
//std::string							m_pvdIPAddress = "127.0.0.1";
//int									m_pvdPortNumber = 5425;
//_uint								m_pvdTimeOutSeconds = 10;