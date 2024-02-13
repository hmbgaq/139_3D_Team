#include "..\Public\PhysXManager.h"

HRESULT CPhysXManager::Initialize()
{
	//m_pPxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_PXErrorCallback);
	//
	//if (nullptr == m_pPxFoundation)
	//	return E_FAIL;

	//m_pPhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pPxFoundation, PxTolerancesScale(), true);



	return S_OK;
}

CPhysXManager* CPhysXManager::Create(_uint iNumLevels)
{
	return nullptr;
}

void CPhysXManager::Free()
{
}
