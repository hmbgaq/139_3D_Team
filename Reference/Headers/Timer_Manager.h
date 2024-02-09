#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer_Manager : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT	Add_Timer(const wstring& strTimeTag);
	_float Compute_TimeDelta(const wstring& strTimeTag);

private:
	class CTimer* Find_Timer(const wstring& strTimeTag) const;	

private:	
	map<const wstring, class CTimer*>		m_Timers;

public:
	static CTimer_Manager* Create();
	virtual void	Free();
};

END