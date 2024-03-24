#include "Mother_State.h"
#include "GameInstance.h"

void CMother_State::Initialize(CMother* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CMother>* CMother_State::Update(CMother* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CMother_State::Release(CMother* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CMother>* CMother_State::Update_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CMother>* CMother_State::Normal_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}


CState<CMother>* CMother_State::Attack_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CMother>* CMother_State::Stun_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CMother>* CMother_State::Normal(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}
