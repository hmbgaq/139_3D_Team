#pragma once
#include "Base.h"
class CCuda_Device : public CBase
{
private:
	CCuda_Device();
	virtual ~CCuda_Device() = default;

public:
	CUcontext* Get_CudaContext();

public:
	HRESULT	Ready_Cuda_Device();
private:
	CUcontext m_pCudaContext = nullptr;
private:
	virtual void Free() override;
};

