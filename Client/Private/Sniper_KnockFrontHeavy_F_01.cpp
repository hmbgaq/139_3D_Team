#include "stdafx.h"
#include "Sniper_KnockFrontHeavy_F_01.h"

void CSniper_KnockFrontHeavy_F_01::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_KnockFrontHeavy_F_01::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CSniper_KnockFrontHeavy_F_01::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
