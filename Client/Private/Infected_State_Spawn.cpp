#include "..\Public\Infected_State_Spawn.h"

void CInfected_State_Spawn::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);
}

CState<CInfected>* CInfected_State_Spawn::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Spawn::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Spawn::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Spawn_State(pActor, fTimeDelta, _iAnimIndex);
}
