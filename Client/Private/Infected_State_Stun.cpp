#include "..\Public\Infected_State_Stun.h"

void CInfected_State_Stun::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);
}

CState<CInfected>* CInfected_State_Stun::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Stun::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Stun::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Stun_State(pActor, fTimeDelta, _iAnimIndex);
}
