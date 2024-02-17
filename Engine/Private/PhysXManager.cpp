#include "..\Public\PhysXManager.h"

HRESULT CPhysXManager::Initialize()
{
	m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_PXErrorCallback);

	//m_Pvd = PxCreatePvd(*m_PxFoundation);
	//The PVD needs connection via a socket. It will run on the Address defined, in our case it's our machine
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(m_pvdIPAddress.c_str(), m_pvdPortNumber, m_pvdTimeOutSeconds);
	//m_Pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true);
	//PxInitExtensions(*m_PhysX, m_Pvd);

	PxSceneDesc sceneDesc(m_PhysX->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f * 10.0f, 0.0f);
	//This creates CPU dispatcher threads or worker threads. We will make 2
	m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_PxDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	//Create the scene now by passing the scene's description
	m_PxScene = m_PhysX->createScene(sceneDesc);


	return S_OK;
}

CPhysXManager* CPhysXManager::Create(_uint iNumLevels)
{
	return nullptr;
}

void CPhysXManager::Free()
{
}
