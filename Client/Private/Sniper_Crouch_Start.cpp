#include "stdafx.h"
#include "Sniper_Crouch_Idle.h"
#include "Sniper_Crouch_Start.h"

void CSniper_Crouch_Start::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_Crouch_Start::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSniper_Crouch_Idle();
	}

	return nullptr;
}

void CSniper_Crouch_Start::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}
