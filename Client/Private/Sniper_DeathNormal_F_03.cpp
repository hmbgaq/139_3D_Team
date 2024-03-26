#include "stdafx.h"
#include "Sniper_DeathNormal_F_03.h"

void CSniper_DeathNormal_F_03::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_DeathNormal_F_03::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Death_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	return nullptr;
}

void CSniper_DeathNormal_F_03::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
