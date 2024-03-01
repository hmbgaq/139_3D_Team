#include "..\Public\Infected_State_Attack.h"

void CInfected_State_Attack::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);
}

CState<CInfected>* CInfected_State_Attack::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Attack::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Attack::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Attack_State(pActor, fTimeDelta, _iAnimIndex);
}
