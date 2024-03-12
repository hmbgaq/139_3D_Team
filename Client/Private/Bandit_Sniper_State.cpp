#include "stdafx.h"
#include "SMath.h"
#include "GameInstance.h"
#include "Bandit_Sniper_State.h"

#include "Sniper_IdlePose.h"
#include "Sniper_Idle.h"
#include "Sniper_CoverHigh_Side_R_Start.h"
#include "Sniper_CoverHigh_Side_L_Start.h"
#include "Sniper_CoverLow_Over_Start.h"
#include "Sniper_CoverLow_Side_L_Start.h"
#include "Sniper_CoverLow_Side_R_Start.h"

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
	return new CSniper_IdlePose();
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Hit_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	/* 애니메이션 혼동방지를 위해 2프레임짜리 Idle로 갔다가 돌아가도록하기 */
	if (pActor->Is_Animation_End())
	{
		return new CSniper_IdlePose();
	}

	return nullptr;
}


CState<CBandit_Sniper>* CBandit_Sniper_State::Death_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return nullptr;
	}

	return nullptr;
}

CState<CBandit_Sniper>* CBandit_Sniper_State::Melee_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	/* Close Attack 상태 */
	CState<CBandit_Sniper>* pState = { nullptr };

	if (pActor->Is_Animation_End())
	{
		return new CSniper_IdlePose();
	}

	return nullptr;
}

/* 중앙제어 */
CState<CBandit_Sniper>* CBandit_Sniper_State::Normal(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CBandit_Sniper>* pState = { nullptr };
	
	/* 움직이질 않는데 ..? */
	//pState = Walk(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;
	//
	//pState = Run(pActor, fTimeDelta, _iAnimIndex);
	//if (pState)	return pState;

	pState = Attack(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

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
CState<CBandit_Sniper>* CBandit_Sniper_State::Attack(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	_bool bPhase = pActor->Get_ProtectExist();

	if (bPhase == true)
	{
		cout << "Start" << endl;
		return new CSniper_CoverHigh_Side_R_Start();

		//return new CSniper_CoverHigh_Side_L_Start();
		//
		//return new CSniper_CoverLow_Over_Start();
		//		   
		//return new CSniper_CoverLow_Side_L_Start();
		//		   
		//return new CSniper_CoverLow_Side_R_Start();
		//		   
		//return new CSniper_Crouch_Start();
	}
	else
	{

	}

	return nullptr;
}
