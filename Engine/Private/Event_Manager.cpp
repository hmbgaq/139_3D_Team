#include "..\Public\Event_Manager.h"

#include "GameInstance.h"
#include "Event.h"

CEvent_Manager::CEvent_Manager()
{
}

HRESULT CEvent_Manager::Initialize()
{
	return S_OK;
}

void CEvent_Manager::AddEventListener(CGameObject* pEventActor, OnEvent OnEvent, Condition Condition)
{
	//m_Events.push_back(new CEvent(pEventActor, OnEvent, Condition));
}

void CEvent_Manager::TriggerEvent()
{
	//for (auto& pEvent : m_Events) {
	//	pEvent->OnEvent();
	//}
}

CEvent_Manager* CEvent_Manager::Create(_uint iNumLevels)
{
	CEvent_Manager* pInstance = new CEvent_Manager;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CEvent_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEvent_Manager::Free()
{
}
