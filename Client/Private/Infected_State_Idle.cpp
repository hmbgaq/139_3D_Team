#include "Infected_State_Idle.h"

void CInfected_State_Idle::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);
}

CState<CInfected>* CInfected_State_Idle::Update(CInfected* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CInfected_State_Idle::Release(CInfected* pActor)
{
	__super::Release(pActor);
}

CState<CInfected>* CInfected_State_Idle::Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	if (pActor->Is_Animation_End()) /* 리스폰 하는 애니메이션이 끝난다면 */
	{
		/* 여기를 안탄다고 ? -> */
		return Normal_State(pActor, fTimeDelta, _iAnimIndex);
	}
	return nullptr;

}
