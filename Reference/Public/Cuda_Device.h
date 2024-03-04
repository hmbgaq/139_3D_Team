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
public:
	CUcontext m_pCudaContext = nullptr;
public:
	static CCuda_Device* Create(const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice,
		_Inout_ ID3D11DeviceContext** ppDeviceContextOut);
	virtual void Free() override;
};

