#include "stdafx.h"
#include "Infected_SpawnClimb_02.h"
#include "Infected_IdleAct_03.h"

void CInfected_SpawnClimb_02::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_SpawnClimb_02::Update(CInfected* pActor, _float fTimeDelta)
{
	//if (pActor->Is_Animation_End()) /* 리스폰 하는 애니메이션이 끝난다면 */
	//{
	//	return new CInfected_IdleAct_03();
	//}
	//return nullptr;

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CInfected_SpawnClimb_02::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
