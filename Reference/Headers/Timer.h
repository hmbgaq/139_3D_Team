#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	HRESULT	Initialize();
	_float Compute_TimeDelta();

private:
	LARGE_INTEGER		m_FrameTime = { 0 };
	LARGE_INTEGER		m_FixTime = { 0 };
	LARGE_INTEGER		m_LastTime = { 0 };
	LARGE_INTEGER		m_CpuTick = { 0 };
	_float				m_fTimeDelta = { 0.f };

public:
	static CTimer* Create();
	virtual void Free();
};

END