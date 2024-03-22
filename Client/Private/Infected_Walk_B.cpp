#include "SMath.h"
#include "stdafx.h"
#include "Infected_Walk_B.h"

#include "Infected_Idle.h"
#include "Infected_IdleAct_01.h"
#include "Infected_IdleAct_02.h"
#include "Infected_IdleAct_03.h"
#include "Infected_RelaxedIdleAct_01.h"
#include "Infected_RelaxedIdleAct_02.h"
#include "Infected_RelaxedIdleAct_03.h"

#include "Body_Infected.h"

void CInfected_Walk_B::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_MonsterAttackState(false);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CBody_Infected* pBody = dynamic_cast<CBody_Infected*>(pActor->Get_Body());
	pBody->Set_RenderState(CBody_Infected::RENDER_STATE::ORIGIN);
}

CState<CInfected>* CInfected_Walk_B::Update(CInfected* pActor, _float fTimeDelta)
{
	pActor->Look_At_Target();

	if (pActor->Is_Animation_End()) /* 공격이 끝나면 무조건 뒷걸음질침 -> Idle -> 공격 -> 뒷걸음질 무한반복  */
	{
		/* Idle 동작으로 바로 연계 */
		_int iRandom = SMath::Random(1, 6);

		switch (iRandom)
		{
		case 1:
			return new CInfected_RelaxedIdleAct_03();
			break;
		case 2:
			return new CInfected_IdleAct_01();
			break;
		case 3:
			return new CInfected_IdleAct_02();
			break;
		case 4:
			return new CInfected_IdleAct_03();
			break;
		case 5:
			return new CInfected_RelaxedIdleAct_01();
			break;
		case 6:
			return new CInfected_RelaxedIdleAct_02();
			break;
		default:
			return new CInfected_IdleAct_01();
			break;
		}
	}

	return nullptr;
}

void CInfected_Walk_B::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
