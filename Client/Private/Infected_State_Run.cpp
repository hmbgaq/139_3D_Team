#include "stdafx.h"
#include "Infected_State_Run.h"

void CInfected_State_Run::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);
}

CState<CInfected>* CInfected_State_Run::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Run::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Run::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Run_State(pActor, fTimeDelta, _iAnimIndex);
}
