#include "..\Public\Hawk_State.h"
#include "GameInstance.h"


void CHawk_State::Initialize(CEdgar* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

}

CState<CEdgar>* CHawk_State::Update(CEdgar* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CHawk_State::Release(CEdgar* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CEdgar>* CHawk_State::Normal_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}


CState<CEdgar>* CHawk_State::Normal(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CHawk_State::Walk(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CHawk_State::Run(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}
