#include "..\Public\Trigger.h"

#include "EventActor.h"

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrigger::CTrigger(const CTrigger& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrigger::Initialize(CEventActor* pActor)
{
	m_pActor = pActor;
	Safe_AddRef(m_pActor);

	return S_OK;
}

void CTrigger::Activate()
{
	m_pActor->Activate();
}

void CTrigger::Free()
{
	__super::Free();
	Safe_Release(m_pActor);
}
