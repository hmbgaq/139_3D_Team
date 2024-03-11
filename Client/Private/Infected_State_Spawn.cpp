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
	if (pActor->Is_Animation_End()) /* 리스폰 하는 애니메이션이 끝난다면 */
	{
		return Spawn_State(pActor, fTimeDelta, _iAnimIndex);;
	}
	return nullptr;

	//return Spawn_State(pActor, fTimeDelta, _iAnimIndex);
}
