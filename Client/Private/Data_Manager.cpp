#include "stdafx.h"
#include "Data_Manager.h"


#include "GameInstance.h"

IMPLEMENT_SINGLETON(CData_Manager);

CData_Manager::CData_Manager()
{
}


void CData_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
