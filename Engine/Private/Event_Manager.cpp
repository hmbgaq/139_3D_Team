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

void CEvent_Manager::Tick(_float fTimeDelta)
{
	for (auto iter = m_Events.begin(); iter != m_Events.end();)
	{
		if (nullptr == (*iter)) 
		{
			iter = m_Events.erase(iter);
		}
			

		if ((*iter)->Activate_Condition())
		{
			(*iter)->Activate();
		}
			
		if ((*iter)->End_Condition()) 
		{
			Safe_Delete(*iter);
			iter = m_Events.erase(iter);
		}
		else 
		{
			++iter;
		}
	}
}

//void CEvent_Manager::TriggerEvent()
//{
//}

CEvent_Manager* CEvent_Manager::Create()
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
	__super::Free();

	for (IEvent* pEvent : m_Events) 
	{
		Safe_Delete(pEvent);
	}
	m_Events.clear();

}
