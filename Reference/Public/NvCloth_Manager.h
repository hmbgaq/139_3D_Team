#pragma once
#include "Base.h"
#include "CustomContextManagerCallback.h"
#include "CustomPxAllocatorCallback.h"
#include "CustomPxAssertHandler.h"
#include "CustomPxErrorCallback.h"

BEGIN(Engine)

class CNvCloth_Manager : public CBase
{
	CNvCloth_Manager();
	virtual ~CNvCloth_Manager() = default;

public:
	Solver* Get_Solver();
	Factory* Get_Factory();
public:
	HRESULT	Initialize();
	void	Tick(_float fTimeDelta);

private:
	CustomContextManagerCallback* m_pGraphicsContextManager{};
	CustomPxAllocatorCallback* m_pAllocatorCallback{};
	CustomPxErrorCallback* m_pErrorCallback{};
	CustomPxAssertHandler* m_pAssertHander{};
	Factory* m_pFactory{};
	Solver* m_pSolver{};



private:
	void Free();
};

END