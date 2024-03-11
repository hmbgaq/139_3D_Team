#include "Bandit_Heavy_State.h"
#include "GameInstance.h"
#include "BanditHeavy_Idle.h"

void CBandit_Heavy_State::Initialize(CBandit_Heavy* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CBandit_Heavy_State::Release(CBandit_Heavy* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Normal_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Walk_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Run_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Attack_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Hit_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Knock_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Death_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}


CState<CBandit_Heavy>* CBandit_Heavy_State::Spawn_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Normal(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Heavy>* pState = { nullptr };

	pState = Idle(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	if (pActor->Is_Animation_End())
	{
		return new CBanditHeavy_Idle();
	}

	return nullptr;
}

CState<CBandit_Heavy>* CBandit_Heavy_State::Idle(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Heavy>* pState = { nullptr };



	return pState;
}

