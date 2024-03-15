#include "..\Public\Infected_State_Finisher.h"

void CInfected_State_Finisher::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);
}

CState<CInfected>* CInfected_State_Finisher::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Finisher::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Finisher::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Finisher_State(pActor, fTimeDelta, _iAnimIndex);
}
