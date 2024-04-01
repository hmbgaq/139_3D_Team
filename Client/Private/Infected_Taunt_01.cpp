#include "stdafx.h"
#include "GameInstance.h"
#include "Infected_Taunt_01.h"

void CInfected_Taunt_01::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CInfected>* CInfected_Taunt_01::Update(CInfected* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		
		return nullptr;
		//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CInfected_Taunt_01::Release(CInfected* pActor)
{
	__super::Release(pActor);
}
