#include "..\Public\Bandit_Sniper_State.h"
#include "GameInstance.h"

#include "Sniper_IdlePose.h"



void CBandit_Sniper_State::Initialize(CBandit_Sniper* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CBandit_Sniper_State::Release(CBandit_Sniper* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Normal_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Sniper>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Walk_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Sniper>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}


CState<CBandit_Sniper>* CBandit_Sniper_State::Cover_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Crouch_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}


CState<CBandit_Sniper>* CBandit_Sniper_State::Hit_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	//CState<CBandit_Sniper>* pState = { nullptr };

	if (pActor->Is_Animation_End())
	{
		//return new CBandit_Sniper_Idle();
	}


	return nullptr;
}


CState<CBandit_Sniper>* CBandit_Sniper_State::Death_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		//return new CBandit_Sniper_Idle();
	}

	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Melee_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Normal(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Sniper>* pState = { nullptr };

	//pState = Dodge(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;


	if (pActor->Is_Animation_End())
	{
		return new CSniper_IdlePose();
	}

	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Walk(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{

	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Run(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{

	return nullptr;
}


