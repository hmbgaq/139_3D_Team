#include "..\Public\Event_Manager.h"

#include "GameInstance.h"

CEvent_Manager::CEvent_Manager()
{
}

HRESULT CEvent_Manager::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	return S_OK;
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
	Safe_Release(m_pGameInstance);
}
