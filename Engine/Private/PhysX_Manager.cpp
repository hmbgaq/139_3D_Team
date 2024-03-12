#include "..\Public\PhysX_Manager.h"

#include "GameInstance.h"
#include "PhysXCollider.h"
#include "PhysXController.h"
#include "CollisionSimulationEventCallBack.h"


CPhysX_Manager::CPhysX_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CPhysX_Manager::Register_PhysXCollider(CPhysXCollider* pPhysXCollider)
{
	m_pPhysXCollders.emplace(pPhysXCollider->Get_PColliderIndex(), pPhysXCollider);
}

CPhysXCollider* CPhysX_Manager::Find_PhysXCollider(const _uint iPhysXColliderIndex)
{
	auto iter = m_pPhysXCollders.find(iPhysXColliderIndex);

	if (iter != m_pPhysXCollders.end())
	{
		return iter->second;
	}

	return nullptr;
}

void CPhysX_Manager::Register_PhysXController(CPhysXController* pPhysXController)
{
	m_pPhysXControllers.emplace(pPhysXController->Get_PControllerIndex(), pPhysXController);
}

CPhysXController* CPhysX_Manager::Find_PhysXController(const _uint iPhysXControllerIndex)
{
	auto iter = m_pPhysXControllers.find(iPhysXControllerIndex);

	if (iter != m_pPhysXControllers.end())
	{
		return iter->second;
	}

	return nullptr;
}

HRESULT CPhysX_Manager::Initialize(const _uint In_iNumLayer)
{
	m_arrCheck.reserve(In_iNumLayer);

	for (_uint i = 0; i < In_iNumLayer; ++i)
	{
		m_arrCheck.emplace_back(0);
	}

	// Create Foundation
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
	if (nullptr == m_pFoundation)
		return E_FAIL;

	//PxRevoluteJointCreate;
	// Create PVD
	char* strTransport = "127.0.0.1";
	m_pPVD = PxCreatePvd(*m_pFoundation);
	PxPvdTransport* Transport = PxDefaultPvdSocketTransportCreate(strTransport, 5425, 10);
	_bool	bPVDConnectionResult = m_pPVD->connect(*Transport, PxPvdInstrumentationFlag::eALL);

	if (!bPVDConnectionResult)
	{
		_int a = 0;
		//MSG_BOX("Faiied to connect to PVD!");
	}

	// Create PhysX
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPVD);
	if (!m_pPhysics)
		return E_FAIL;

	// Create Cooking
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams(PxTolerancesScale()));
	if (!m_pCooking)
		return E_FAIL;

	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, -10.f);
	if (!m_pMaterial)
		return E_FAIL;

	m_pCollisionSimulationEventCallBack = new CollisionSimulationEventCallBack();

	Create_Scene();

	return S_OK;
}

void CPhysX_Manager::Tick(_float fTimeDelta)
{
	if (m_fTimeAcc > 1.f)
	{
		Garbage_Collector();
		m_fTimeAcc = 0.f;
	}
	else
	{
		m_fTimeAcc += fTimeDelta;
	}

	if (m_pScene)
	{
		if (fTimeDelta > 3.f)
			return;

		if (1 == m_pScene->getTimestamp() ||
			2 == m_pScene->getTimestamp())
			fTimeDelta = 0.16f;

		m_pScene->simulate(fTimeDelta);
		m_pScene->fetchResults(true);
	}

}

void CPhysX_Manager::Check_PhysXFilterGroup(const _uint In_iLeftLayer, const _uint In_iRightLayer)
{
	_uint iRow = (_uint)In_iLeftLayer;
	_uint iCol = (_uint)In_iRightLayer; 

	_uint iMax = iCol;
	if (iRow > iCol)
	{
		iMax = iRow;
	}

	for (_uint i = (_uint)m_arrCheck.size(); i <= iMax; ++i)
	{
		m_arrCheck.emplace_back(0);
	}

	if (m_arrCheck[iRow] & (1 << iCol))
	{
		m_arrCheck[iRow] &= ~(1 << iCol); 
		m_arrCheck[iCol] &= ~(1 << iRow); 
	}
	else
	{
		m_arrCheck[iRow] |= (1 << iCol);
		m_arrCheck[iCol] |= (1 << iRow);
	}
}

_uint CPhysX_Manager::Get_PhysXFilterGroup(const _uint In_iIndex)
{
	return m_arrCheck[In_iIndex];
}

PxFilterFlags CollisionFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	if (filterData0.word2 != 1 || filterData1.word2 != 1)
	{
		return PxFilterFlag::eKILL;
	}

	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{

		pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;
		pairFlags |= PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		return PxFilterFlag::eDEFAULT;
	}
	else
	{
		return PxFilterFlag::eKILL;
	}

	return PxFilterFlag::eDEFAULT;
}

HRESULT CPhysX_Manager::Create_Scene(PxVec3 Gravity)
{
	// Set Scene
	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = Gravity;

	// Set Dispatcher
	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.filterShader = CollisionFilterShader;	
	//sceneDesc.cudaContextManager = m_pCudaContextManager;
	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;	//Enable GPU dynamics - without this enabled, simulation (contact gen and solver) will run on the CPU.
	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;			//Enable PCM. PCM NP is supported on GPU. Legacy contact gen will fall back to CPU
	sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;	//Improve solver stability by enabling post-stabilization.
	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;		//Enable GPU broad phase. Without this set, broad phase will run on the CPU.
	sceneDesc.gpuMaxNumPartitions = 8;						//Defines the maximum number of partitions used by the solver. Only power-of-2 values are valid. 
															//A value of 8 generally gives best balance between performance and stability.

	m_pScene = m_pPhysics->createScene(sceneDesc);
	if (!m_pScene)
		return E_FAIL;

	PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	if (nullptr == m_pCollisionSimulationEventCallBack)
		return E_FAIL;

	m_pScene->setSimulationEventCallback(m_pCollisionSimulationEventCallBack);

	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 10.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eBODY_ANG_VELOCITY, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eBODY_AXES, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eBODY_LIN_VELOCITY, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eBODY_MASS_AXES, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AABBS, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AXES, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_COMPOUNDS, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_DYNAMIC, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_EDGES, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_FNORMALS, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_STATIC, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_ERROR, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_FORCE, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_NORMAL, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_POINT, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCULL_BOX, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eFORCE_DWORD, 20.0f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eWORLD_AXES, 20.0f);





	if (m_pControllerManager)
		m_pControllerManager->release();

	m_pControllerManager = PxCreateControllerManager(*m_pScene);

	if (!m_pControllerManager)
		return E_FAIL;


	return S_OK;
}

HRESULT CPhysX_Manager::Delete_Scene()
{
	if (m_pScene)
		m_pScene->release();
	m_pScene = nullptr;

	if (m_pDispatcher)
		m_pDispatcher->release();

	m_pDispatcher = nullptr;

	return S_OK;
}

PxRigidDynamic* CPhysX_Manager::Create_DynamicActor(const PxTransform& transform, const PxGeometry& geometry, PxMaterial* pMaterial)
{
	return PxCreateDynamic(*m_pPhysics, transform, geometry, *pMaterial, 10.f);
}

PxRigidDynamic* CPhysX_Manager::Create_DynamicActor(const PxTransform& transform)
{
	return m_pPhysics->createRigidDynamic(transform);
}

PxRigidStatic* CPhysX_Manager::Create_StaticActor(const PxTransform& transform, const PxGeometry& geometry, PxMaterial* pMaterial)
{
	return PxCreateStatic(*m_pPhysics, transform, geometry, *pMaterial);
}

PxRigidStatic* CPhysX_Manager::Create_StaticActor(const PxTransform& transform)
{
	return m_pPhysics->createRigidStatic(transform);
}

void CPhysX_Manager::Add_DynamicActorAtCurrentScene(PxRigidDynamic& DynamicActor)
{
	m_pScene->addActor(DynamicActor);
}

void CPhysX_Manager::Add_StaticActorAtCurrentScene(PxRigidStatic& StaticActor)
{
	m_pScene->addActor(StaticActor);
}

void CPhysX_Manager::Create_Material(_float fStaticFriction, _float fDynamicFriction, _float fRestitution, PxMaterial** ppOut)
{
	*ppOut = m_pPhysics->createMaterial(fStaticFriction, fDynamicFriction, fRestitution);
}

void CPhysX_Manager::Create_ConvexMesh(PxVec3** pVertices, _uint iNumVertice, PxConvexMesh** ppOut)
{
	PxCookingParams params = m_pCooking->getParams();

	// Use the new (default) PxConvexMeshCookingType::eQUICKHULL
	params.convexMeshCookingType = PxConvexMeshCookingType::eQUICKHULL;

	// If the gaussMapLimit is chosen higher than the number of output vertices, no gauss map is added to the convex mesh data (here 256).
	// If the gaussMapLimit is chosen lower than the number of output vertices, a gauss map is added to the convex mesh data (here 16).
	params.gaussMapLimit = 16;
	params.buildGPUData = true;
	m_pCooking->setParams(params);

	// Setup the convex mesh descriptor
	PxConvexMeshDesc Desc;

	// We provide points only, therefore the PxConvexFlag::eCOMPUTE_CONVEX flag must be specified
	Desc.points.data = *pVertices;
	Desc.points.count = iNumVertice;
	Desc.points.stride = sizeof(PxVec3);
	Desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	*ppOut = m_pCooking->createConvexMesh(Desc, m_pPhysics->getPhysicsInsertionCallback());
}

void CPhysX_Manager::Create_ConvexMesh(const PxConvexMeshDesc& In_MeshDesc, PxConvexMesh** ppOut)
{
	PxCookingParams params = m_pCooking->getParams();

	// Use the new (default) PxConvexMeshCookingType::eQUICKHULL
	params.convexMeshCookingType = PxConvexMeshCookingType::eQUICKHULL;

	// If the gaussMapLimit is chosen higher than the number of output vertices, no gauss map is added to the convex mesh data (here 256).
	// If the gaussMapLimit is chosen lower than the number of output vertices, a gauss map is added to the convex mesh data (here 16).
	params.gaussMapLimit = 16;
	params.buildGPUData = true;
	m_pCooking->setParams(params);

	*ppOut = m_pCooking->createConvexMesh(In_MeshDesc, m_pPhysics->getPhysicsInsertionCallback());

}

void CPhysX_Manager::Create_Shape(const PxGeometry& Geometry, PxMaterial* pMaterial, const _bool isExculsive, const PxShapeFlags In_ShapeFlags, PxShape** ppOut)
{
	*ppOut = m_pPhysics->createShape(Geometry, *pMaterial, isExculsive, In_ShapeFlags);
}

void CPhysX_Manager::Create_MeshFromTriangles(const PxTriangleMeshDesc& In_MeshDesc, PxTriangleMesh** ppOut)
{
	PxCookingParams params = m_pCooking->getParams();

	// Use the new (default) PxConvexMeshCookingType::eQUICKHULL
	params.buildGPUData = true;

	m_pCooking->setParams(params);


	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = m_pCooking->cookTriangleMesh(In_MeshDesc, writeBuffer, &result);
	//if (!status)
		//DEBUG_ASSERT;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	*ppOut = m_pPhysics->createTriangleMesh(readBuffer);
}

void CPhysX_Manager::Create_Controller(const PxCapsuleControllerDesc& In_ControllerDesc, PxController** ppOut)
{
	*ppOut = m_pControllerManager->createController(In_ControllerDesc);
}


void CPhysX_Manager::Garbage_Collector()
{
	for (auto iter = m_pPhysXCollders.begin(); iter != m_pPhysXCollders.end();)
	{
		if (nullptr == (*iter).second)
		{
			iter = m_pPhysXCollders.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	for (auto iter = m_pPhysXControllers.begin(); iter != m_pPhysXControllers.end();)
	{
		if (nullptr == (*iter).second)
		{
			iter = m_pPhysXControllers.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

CPhysX_Manager* CPhysX_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint In_iNumLayer)
{
	CPhysX_Manager* pInstance = new CPhysX_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize(In_iNumLayer)))
	{
		MSG_BOX("Failed to Created : CPhysX_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPhysX_Manager::Free()
{
	PX_UNUSED(true);

	if (m_pControllerManager)
		m_pControllerManager->release();

	if (m_pScene)
		m_pScene->release();

	if (m_pPhysics)
		m_pPhysics->release();

	if (m_pCooking)
		m_pCooking->release();

	if (m_pPVD)
	{
		PxPvdTransport* transport = m_pPVD->getTransport();
		m_pPVD->release();
		m_pPVD = nullptr;
		transport->release();
	}

	if (m_pFoundation)
		m_pFoundation->release();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pCollisionSimulationEventCallBack->Release();
}
