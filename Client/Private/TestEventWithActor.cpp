#include "..\Public\TestEventWithActor.h"

CTestEventWithActor::CTestEventWithActor()
{
}

void CTestEventWithActor::Activate()
{
	if (nullptr == m_pActor) 
	{
		//m_bIsEnd = true;
		return;
	}
		

	m_pActor->Activate();
}

_bool CTestEventWithActor::Activate_Condition()
{
	if (nullptr == m_pActor) 
	{
		return false;
	}
		
	return m_pActor->Activate_Condition();
}

_bool CTestEventWithActor::End_Condition()
{
	if (nullptr == m_pActor)
	{
		return true;
	}

	return m_pActor->End_Condition();
}

CTestEventWithActor* CTestEventWithActor::Create(IEvent* pActor)
{
	CTestEventWithActor* pInstance = new CTestEventWithActor();
	//m_pActor = pActor;
	pInstance->Set_Actor(pActor);
	return pInstance;
}

void CTestEventWithActor::Free()
{

}
