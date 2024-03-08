// #include "NvCloth_Manager.h"
// #include "GameInstance.h"
// #include <cuda.h>
// 
// IMPLEMENT_SINGLETON(CNvCloth_Manager)
// 
// Solver* CNvCloth_Manager::Get_Solver()
// {
// 	return m_pSolver;
// }
// 
// Factory* CNvCloth_Manager::Get_Factory()
// {
// 	return m_pFactory;
// }
// 
// HRESULT CNvCloth_Manager::Initialize()
// {
// 	//CUcontext cudaContext;
// 	///*int deviceCount = 0;
// 	//CUresult result = cuDeviceGetCount(&deviceCount);
// 	//assert(CUDA_SUCCESS == result);
// 	//assert(deviceCount >= 1);*/
// 	//
// 
// 	//CUresult result = cuCtxCreate(&cudaContext, 0, 0); //Pick first device
// 
// 
// 	m_pGraphicsContextManager = DBG_NEW CustomContextManagerCallback();
// 
// 	m_pGraphicsContextManager->mContext = DEVICECONTEXT;
// 
// 	m_pAllocatorCallback = DBG_NEW CustomPxAllocatorCallback();
// 	m_pErrorCallback = DBG_NEW CustomPxErrorCallback();
// 	m_pAssertHander = DBG_NEW CustomPxAssertHandler();
// 
// 	InitializeNvCloth(m_pAllocatorCallback, m_pErrorCallback, m_pAssertHander, nullptr);
// 
// 	if (CHECK_CUDA)
// 	{
// 		m_pFactory = NvClothCreateFactoryCUDA(*GET_SINGLE(CCuda_Device)->Get_CudaContext());
// 	}
// 	else
// 	{
// 		m_pFactory = NvClothCreateFactoryDX11(m_pGraphicsContextManager);
// 	}
// 
// 	//m_pFactory = NvClothCreateFactoryCPU();
// 
// 	//
// 	//We need to release all DX objects after destroying the factory.
// 
// 	m_pSolver = m_pFactory->createSolver();
// 
// 	return S_OK;
// }
// 
// void CNvCloth_Manager::Tick(_float fTimeDelta)
// {
// 	if (m_pSolver->getSimulationChunkCount() <= 0)
// 		return;
// 
// 	shared_ptr<CThread_Manager> pThread_Manager = GET_SINGLE(CThread_Manager);
// 
// 	m_pGraphicsContextManager->mDevice = DEVICE;
// 	m_pGraphicsContextManager->mContext = DEVICECONTEXT;
// 	m_pGraphicsContextManager->mSynchronizeResources = false;
// 	m_pSolver->beginSimulation(fTimeDelta);
// 	for (int i = 0; i < m_pSolver->getSimulationChunkCount(); i++)
// 	{
// 		//pThread_Manager->Enqueue_Job(bind(&Solver::simulateChunk, m_pSolver, i));
// 		m_pSolver->simulateChunk(i);
// 	}
// 
// 	//pThread_Manager->Wait_JobDone();
// 
// 	m_pSolver->endSimulation();
// 
// }
// 
// void CNvCloth_Manager::Free()
// {
// 	Safe_Delete(m_pSolver);
// 	NvClothDestroyFactory(m_pFactory);
// 	Safe_Delete(m_pGraphicsContextManager);
// 	Safe_Delete(m_pAllocatorCallback);
// 	Safe_Delete(m_pErrorCallback);
// 	Safe_Delete(m_pAssertHander);
// }
