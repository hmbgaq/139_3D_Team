#include "stdafx.h"
#include "GameInstance.h"
#include "Infected_Sprint_F_Melee_02.h"
#include "Infected_Walk_B.h"

void CInfected_Sprint_F_Melee_02::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_Sprint_F_Melee_02::Update(CInfected* pActor, _float fTimeDelta)
{
	/* 긴 거리 공격 */
	if (pActor->Is_Animation_End())
	{
		/* 달리기 끝났으니 원래 스피드 복귀 */
		pActor->Get_Transform()->Set_Speed(10.f);
		return new CInfected_Walk_B();
	}

	return nullptr;
}

void CInfected_Sprint_F_Melee_02::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
