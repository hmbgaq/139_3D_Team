#include "stdafx.h"
#include "Sniper_HitLightOpened_L01.h"

void CSniper_HitLightOpened_L01::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_HitLightOpened_L01::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CSniper_HitLightOpened_L01::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
