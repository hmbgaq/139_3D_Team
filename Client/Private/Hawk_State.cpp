#include "..\Public\Hawk_State.h"
#include "GameInstance.h"


void CHawk_State::Initialize(CHawk* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

}

CState<CHawk>* CHawk_State::Update(CHawk* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CHawk_State::Release(CHawk* pActor)
{
	Safe_Release(m_pGameInstance);
}

//CState<CHawk>* CHawk_State::Normal_State(CHawk* pActor, _float fTimeDelta, _uint _iAnimIndex)
//{
//	return Normal(pActor, fTimeDelta, _iAnimIndex);
//}
//
//
//CState<CHawk>* CHawk_State::Normal(CHawk* pActor, _float fTimeDelta, _uint _iAnimIndex)
//{
//	return nullptr;
//}
//
//CState<CHawk>* CHawk_State::Walk(CHawk* pActor, _float fTimeDelta, _uint _iAnimIndex)
//{
//	return nullptr;
//}
//
//CState<CHawk>* CHawk_State::Run(CHawk* pActor, _float fTimeDelta, _uint _iAnimIndex)
//{
//	return nullptr;
//}
