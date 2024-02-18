#include "stdafx.h"
#include "TestEvent.h"

CTestEvent::CTestEvent()
{
}



void CTestEvent::Activate()
{
    _bool test = false;
    test = true;
}

_bool CTestEvent::Activate_Condition()
{
    return true;
}

_bool CTestEvent::End_Condition()
{
    return true;
}

CTestEvent* CTestEvent::Create()
{
	CTestEvent* pInstance = new CTestEvent();

	///* 원형객체를 초기화한다.  */
	//if (FAILED(pInstance->Initialize_Prototype()))
	//{
	//	MSG_BOX("Failed to Created : CPlayer");
	//	Safe_Release(pInstance);
	//}
	return pInstance;
}

void CTestEvent::Free()
{
}
