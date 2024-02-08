#include "..\Public\Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

HRESULT CTimer_Manager::Initialize()
{
	return	S_OK;
}

HRESULT CTimer_Manager::Add_Timer(const wstring& strTimeTag)
{
	if (nullptr != Find_Timer(strTimeTag))
		return E_FAIL;

	CTimer*	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.insert({ strTimeTag, pTimer });

	return S_OK;
}


_float CTimer_Manager::Compute_TimeDelta(const wstring& strTimeTag)
{
	CTimer*		pTimer = Find_Timer(strTimeTag);
	if (nullptr == pTimer)
		return 0.0f;

	return pTimer->Compute_TimeDelta();
}


CTimer * CTimer_Manager::Find_Timer(const wstring& strTimeTag) const
{
	auto	iter = m_Timers.find(strTimeTag);	
	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

CTimer_Manager * CTimer_Manager::Create()
{
	CTimer_Manager*		pInstance = new CTimer_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTimer_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_Timers)
		Safe_Release(Pair.second);
	m_Timers.clear();	
}
