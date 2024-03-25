#include "stdafx.h"
#include "Sniper_HitHeavy_FL.h"

void CSniper_HitHeavy_FL::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_HitHeavy_FL::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CSniper_HitHeavy_FL::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
