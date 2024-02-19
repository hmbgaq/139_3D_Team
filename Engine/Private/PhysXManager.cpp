#include "..\Public\PhysXManager.h"

#include "PhysXCollider.h"
#include "PhysXController.h"


CPhysXManager::CPhysXManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
}

void CPhysXManager::Register_PhysXCollider(CPhysXCollider* pPhysXCollider)
{
	m_pPhysXCollders.emplace(pPhysXCollider->Get_PColliderIndex(), pPhysXCollider);
}

CPhysXCollider* CPhysXManager::Find_PhysXCollider(const _uint iPhysXColliderIndex)
{
	return nullptr;
}

void CPhysXManager::Register_PhysXController(CPhysXController* pPhysXController)
{
}

CPhysXController* CPhysXManager::Find_PhysXController(const _uint iPhysXControllerIndex)
{
	return nullptr;
}

HRESULT CPhysXManager::Initialize(const _uint In_iNumLayer)
{
	m_arrCheck.reserve(In_iNumLayer);

	for (_uint i = 0; i < In_iNumLayer; ++i)
	{
		m_arrCheck.emplace_back(0);
	}

	// Create Foundation
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
	PxRevoluteJointCreate;
	// Create PVD
	char* strTransport = "127.0.0.1";
	m_pPVD = PxCreatePvd(*m_pFoundation);
	PxPvdTransport* Transport = PxDefaultPvdSocketTransportCreate(strTransport, 5425, 10);
	_bool	bPVDConnectionResult = m_pPVD->connect(*Transport, PxPvdInstrumentationFlag::eALL);
	if (!bPVDConnectionResult)
	{
		//MSG_BOX("Faiied to connect to PVD!");
	}

	// Create PhysX
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPVD);

	// Create Cooking
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams(PxTolerancesScale()));

	// Create Cuda
	PxCudaContextManagerDesc tCudaDesc;
	//tCudaDesc.graphicsDevice = DEVICE;
	//tCudaDesc.interopMode = PxCudaInteropMode::Enum::D3D11_INTEROP;
	//tCudaDesc.ctx = GET_SINGLE(CCuda_Device)->Get_CudaContext();

	m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, tCudaDesc, PxGetProfilerCallback());

	if (m_pCudaContextManager)
	{
		if (!m_pCudaContextManager->contextIsValid())
		{
			if (m_pCudaContextManager)
				m_pCudaContextManager->release();
			m_pCudaContextManager = nullptr;
		}
	}


	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, -10.f);





	return S_OK;
}

void CPhysXManager::Tick(_float fTimeDelta)
{
}

CPhysXManager* CPhysXManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumLevels)
{
	return nullptr;
}

void CPhysXManager::Free()
{
}
