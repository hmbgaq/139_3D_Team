#include "Son_State.h"
#include "GameInstance.h"

void CSon_State::Initialize(CSon* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CSon>* CSon_State::Update(CSon* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CSon_State::Release(CSon* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CSon>* CSon_State::Update_State(CSon* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CSon>* CSon_State::Normal_State(CSon* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CSon>* CSon_State::Attack_State(CSon* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CSon>* CSon_State::Normal(CSon* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}
