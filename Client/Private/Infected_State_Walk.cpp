#include "stdafx.h"
#include "Infected_IdlePose.h"
#include "Infected_State_Walk.h"

void CInfected_State_Walk::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);
}

CState<CInfected>* CInfected_State_Walk::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Walk::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Walk::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return new CInfected_IdlePose();
	
	//return Walk_State(pActor, fTimeDelta, _iAnimIndex);
}

CState<CInfected>* CInfected_State_Walk::PreAttack_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Attack_State(pActor, fTimeDelta, _iAnimIndex);
}

CState<CInfected>* CInfected_State_Walk::PreRun_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Run_State(pActor, fTimeDelta, _iAnimIndex);
}
