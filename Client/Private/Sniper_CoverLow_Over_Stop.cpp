#include "stdafx.h"
#include "Sniper_CoverLow_Over_Stop.h"
#include "Sniper_CoverLow_Idle.h"

void CSniper_CoverLow_Over_Stop::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);


	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_Monster_State(CMonster_Character::Monster_State::ATTACK);
}

CState<CBandit_Sniper>* CSniper_CoverLow_Over_Stop::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	//if (pActor->Is_Inputable_Front(20) && false == m_bFlags[0])
	//{
	//	pActor->Set_ColliderSize(0.4f, 0.6f, 0.4f);
	//	m_bFlags[0] = true;
	//}

	if (pActor->Is_Animation_End())
	{
		return new CSniper_CoverLow_Idle();
	}

	return nullptr;
}

void CSniper_CoverLow_Over_Stop::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);

	pActor->Set_Monster_State(CMonster_Character::Monster_State::Monster_State_End);
}
