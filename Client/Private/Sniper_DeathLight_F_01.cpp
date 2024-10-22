#include "stdafx.h"
#include "Sniper_DeathLight_F_01.h"

void CSniper_DeathLight_F_01::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Get_Body()->Collider_Off();
}

CState<CBandit_Sniper>* CSniper_DeathLight_F_01::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return __super::Death_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CSniper_DeathLight_F_01::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
