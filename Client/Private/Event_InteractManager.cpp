#include "..\Public\Event_InteractManager.h"
#include "Player.h"

CEvent_InteractManager::CEvent_InteractManager()
{
}

void CEvent_InteractManager::Activate()
{
	if (nullptr == m_pActor)
	{
		//m_bIsEnd = true;
		return;
	}

	
}

_bool CEvent_InteractManager::Activate_Condition()
{
	if (nullptr == m_pActor)
	{
		return false;
	}

	return false;
	//return m_pActor->Activate_Condition();
}

_bool CEvent_InteractManager::End_Condition()
{
	if (nullptr == m_pActor)
	{
		return true;
	}

	return false;
	//return m_pActor->End_Condition();
}

CEvent_InteractManager* CEvent_InteractManager::Create(CPlayer* pActor)
{
	CEvent_InteractManager* pInstance = new CEvent_InteractManager();
	//m_pActor = pActor;
	pInstance->Set_Actor(pActor);
	return pInstance;
}

void CEvent_InteractManager::Free()
{

}
