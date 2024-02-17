#include "..\Public\Event.h"

#include "GameObject.h"

CEvent::CEvent(CGameObject* pEventActor, OnEvent OnEvnet, Condition Condition)
	: m_pEventActor(pEventActor)
	, m_OnEvnet(OnEvnet)
	, m_Condition(Condition)
{
	Safe_AddRef(m_pEventActor);
}

_bool CEvent::Activate()
{
	//_bool is_Activate = Condition();
	//if (is_Activate)
	//{
	//	m_pEventActor->OnEvent();
	//}

	//return is_Activate;
	return false;
}

void CEvent::Free()
{
	__super::Free();

	Safe_Release(m_pEventActor);
}
