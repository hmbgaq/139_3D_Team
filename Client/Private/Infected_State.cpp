#include "..\Public\Infected_State.h"

#include "Infected_Idle.h"
#include "Infected_IdlePose.h"
#include "Infected_RelaxedIdleAct_01.h"

#include "Infected_Walk_B.h"
#include "Infected_Walk_F.h"
#include "Infected_Walk_FL.h"
#include "Infected_Walk_FL45.h"
#include "Infected_Walk_FR.h"
#include "Infected_Walk_FR.h"
//#include "Infected_Walk_B.h"
//#include "Infected_Walk_B.h"


void CInfected_State::Initialize(CInfected* pActor)
{
}

CState<CInfected>* CInfected_State::Update(CInfected* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CInfected_State::Release(CInfected* pActor)
{
}

CState<CInfected>* CInfected_State::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CInfected>* CInfected_State::Normal_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CInfected>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CInfected>* CInfected_State::Walk_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CInfected>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CInfected>* CInfected_State::Run_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	CState<CInfected>* pState = { nullptr };

	pState = Normal(pActor, fTimeDelta, _iAnimIndex);
	if (pState)	return pState;

	return nullptr;
}

CState<CInfected>* CInfected_State::Attack_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CInfected>* CInfected_State::Hit_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CInfected>* CInfected_State::Knock_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CInfected>* CInfected_State::Dodge_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Spawn_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Death_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Normal(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CInfected>* CInfected_State::Walk(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CInfected>* CInfected_State::Run(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CInfected>* CInfected_State::Attack(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CInfected>* CInfected_State::Hit(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Knock(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Dodge(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}
	return nullptr;
}

CState<CInfected>* CInfected_State::Spawn(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}

CState<CInfected>* CInfected_State::Death(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End())
	{
		return new CInfected_Idle();
	}

	return nullptr;
}
