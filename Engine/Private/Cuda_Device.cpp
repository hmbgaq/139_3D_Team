#include "Cuda_Device.h"

CCuda_Device::CCuda_Device()
{
}

CUcontext* CCuda_Device::Get_CudaContext()
{
	return &m_pCudaContext;
}

HRESULT CCuda_Device::Ready_Cuda_Device()
{
	cuInit(0);

	CUresult result = cuCtxCreate(&m_pCudaContext, 0, 0);

	return S_OK;
}

CCuda_Device* CCuda_Device::Create(const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContextOut)
{
	return nullptr;
}

void CCuda_Device::Free()
{
	cuCtxDestroy(m_pCudaContext);
}
