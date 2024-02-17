#include "..\Public\PhysXManager.h"

#include "Model.h"
#include "Transform.h"



HRESULT CPhysXManager::Initialize()
{
	m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_PXErrorCallback);
	m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true);

	PxSceneDesc sceneDesc(m_PhysX->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f * 10.0f, 0.0f);

	m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_PxDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	m_PxScene = m_PhysX->createScene(sceneDesc);

	return S_OK;
}

void CPhysXManager::Late_Tick(_float fTimeDelta)
{
	//Update_Branches();

	m_PxScene->simulate(fTimeDelta);
	m_PxScene->fetchResults(true);

	//Set_BranchesToBone();
}

//void CPhysXManager::Update_Branches()
//{
//	for (auto& Player : m_PlayerInfos)
//	{
//		CModel* pModel = Player.pPlayerModel;
//		_matrix matPivot = pModel->Get_PivotMatrix();
//		_matrix matPlayerWorld = Player.pPlayerTransform->Get_WorldMatrix();
//
//		for (auto& Branch : Player.Branches)
//		{
//			_matrix matFirst = pModel->Get_CombinedMatrix(Branch.Bones[0]) * matPivot * matPlayerWorld;
//			Branch.Frames[0]->setKinematicTarget(MatrixToPxTrans(matFirst));
//		}
//	}
//}
//
//void CPhysXManager::Set_BranchesToBone()
//{
//}
//
//void CPhysXManager::Add_Player(CGameObject* pPlayer)
//{
//}
//
//void CPhysXManager::Add_BoneBranch(CGameObject* pPlayer, vector<_uint>& Bones)
//{
//}

void CPhysXManager::Reset()
{
}

CPhysXManager* CPhysXManager::Create(_uint iNumLevels)
{
	return nullptr;
}

void CPhysXManager::Free()
{
}
